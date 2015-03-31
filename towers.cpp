#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/tower.h"
#include "include/network.h"
#include "include/protocol.h"

/*Tower processes related*/
extern pid_t towerprocesses[MAX_TOWERS];
extern key_t msgid[MAX_TOWERS];
extern int towercount;
extern pid_t mainprocessid;

/*routing related*/
extern void networkoperations(Tower *me);

/*Debug related*/
extern void print_networkinfo(Tower * me);

extern void updatePendingAck(Tower* me, int message_id,void* pkt, int size);
extern void opendb(Tower* me);
extern void closedb(Tower* me);
extern int internalhash(int msgid, int type, int origin);
extern int readTableUpdatepacket(message_nwt_buf *rbuf,int msqid);
extern int readJoinpacket(message_join_buf *rbuf,int msqid);
extern int readReliabilitypacket(message_rel_buf *rbuf,int msqid);

extern void sendTableUpdatepacket(message_nwt_buf* sbuf,int rxid);
extern void sendJoinpacket(message_join_buf* sbuf,int rxid);
extern void sendReliabilitypacket(message_rel_buf* sbuf,int rxid);
extern void mergetable(double rcvdtable[MAX_TOWERS][MAX_TOWERS],Tower* me,int senderID);

void Tower_run(Tower* me)
{    
    assert(towercount>0 && towercount <MAX_TOWERS);
	assert(me->state == TOWER_JOINED);
	LOGD("Tower Begining to function .... \n");
    while(1)
    {
        networkoperations(me);
    }
    closedb(me);
}

void Tower_init(Tower* me)
{
	int msgflg = IPC_CREAT|0644;
	/*Others can read only I can write and read and create!*/

    LOGI("Tower init ... Creating inbox for tower %d and memorizing Inbox of towers created till now (for sending hello broadcast only) \n",towercount-1);

    /*init others*/
    me->myID = towercount-1;
    
    memset(me->algoinfo.alltowers,0,sizeof(me->algoinfo.alltowers));
    for(int i=0;i<MAX_TOWERS;i++)
        me->lastrcvd[i] = -1;

    for(int i=0; i<MAX_TOWERS; i++)
        for(int j=0; j<MAX_TOWERS; j++)
            me->algoinfo.routingtable[i][j] = 0.0;
    me->message_no = 0;

    if((me->myInboxId = msgget(msgid[towercount-1],msgflg))<=0)
    {
        perror("msgget failed");
        LOGD("init failed :-( .... \n")
        exit(1);

    }
    else
        LOGD(" Created Inbox from msgid[towercount-1] %d , my InboxId %d",msgid[towercount-1],me->myInboxId)
    msgflg = 0644;
	/*I can write and read other queue (permisson to be changed)*/
	msgflg = 0244;
	/*I should not be able to read the queue; other processes I dont care! */


	message_join_buf packet , rcv;
    packet.messagetype = JOIN;
	packet.join.message_id = internalhash(me->message_no, packet.messagetype, packet.join.senderID);
    packet.join.ack = 0;
    packet.join.sendersInboxId = me->myInboxId;
    packet.join.locx = me->locx;
    packet.join.locy = me->locy;
	packet.join.senderID = packet.join.OriginID = me->myID;
	me->message_no++;
    opendb(me);

	for(int i=0; i<towercount-1; i++)
    {
        if((me->algoinfo.alltowers[i] = msgget(msgid[i],msgflg))<=0)
        {
			perror("msgget failed");
            LOGD("msgget failed")
            exit(1);
        }
		sendJoinpacket(&packet,me->algoinfo.alltowers[i]);
    }
	/*wait till others write*/
	LOGD("sleeping for %d secs hope all other processes run now !! \n",max(towercount*3,10));
	sleep(max(towercount*3,10));
    bool rec = false;
	for(int i=0;i<MAX_TOWERS;i++) {
		if(readJoinpacket(&rcv,me->myInboxId)>0) {
            rec = true;
			assert(rcv.join.ack == 1 && rcv.join.message_id == packet.join.message_id);
			me->algoinfo.neighbours[packet.join.senderID] = packet.join.sendersInboxId;
			mergetable(rcv.join.table,me,packet.join.senderID);
		}
	}
    if(rec) {
	   /*MakeNetworkTableUpdatePacket*/
    	message_nwt_buf nwtpacket;
    	nwtpacket.messagetype = NWTABLE;
    	nwtpacket.nwt.message_id = internalhash(me->message_no, packet.messagetype,packet.join.OriginID );
    	nwtpacket.nwt.ack = 0;
    	nwtpacket.nwt.sendersInboxId = me->myInboxId;
    	nwtpacket.nwt.senderID = me->myID;
    	memcpy(nwtpacket.nwt.table,me->algoinfo.routingtable,sizeof(nwtpacket.nwt.table));
    	for(int i=0;i<MAX_TOWERS;i++) {
    		if(nwtpacket.nwt.table[me->myID][i]>0) {
    				assert(me->algoinfo.neighbours[i] !=0);
    				sendTableUpdatepacket(&nwtpacket, me->algoinfo.neighbours[i]);
                    /*Expecting an ACK; Save the message in DB for retransmission*/
    				//updatePendingAck(me, nwtpacket.nwt.message_id,(void*) &nwtpacket, sizeof(nwtpacket));
    		}
    	}

        /*retry till I get ack from my friends should you */
        //retryTableUpdate(callback);
    	
        LOGD(" sleeping .. let the update propogate ------ %d",towercount*AVGRTT_PER_TOWER*5)
    	sleep(towercount*AVGRTT_PER_TOWER*7);
    	LOGD("Tower initialized . Network is stable ... can start functioning \n");
    }   
	/*stability signal to parent only*/
    kill(mainprocessid,SIGUSR1);
}


/*Create tower process*/
void create_tower_process(Tower tower)
{

    /*create here*/
    pid_t cpid= fork();

    towerprocesses[towercount++] = cpid; /*This variable makes sense in parent process*/

    switch(cpid)
    {
    case 0:
    {
        LOGD("New tower process created with pid %ld\n",(long)getpid());
        Tower_init(&tower);
        Tower_run(&tower); /*this is an*/
        break;
    }

    case -1:
    {
        printf("Unable to fork() \n");
        break;

    }

    default:
        LOGD("Im the parent/ main process \n");


    }

}
