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

#include "tower.h"
#include "network.h"




/*Tower processes related*/
extern pid_t towerprocesses[MAX_TOWERS];
extern key_t msgid[MAX_TOWERS];
extern int towercount;
extern pid_t mainprocessid;


struct  message_buf
{
    long messagetype;
    struct msg
    {
        char ack; // to have read reciept
        int message_id;
        int locx;
        int locy;
        int sendersInboxId;
    } message;

} ;


void makedscoverypacket(Tower* me, message_buf* packet)
{

    packet->messagetype = DISCOVERY;
    packet->message.ack = 0;
    packet->message.sendersInboxId = me->myInboxId;
    assert(packet->message.sendersInboxId == me->myInboxId);
    packet->message.locx = me->locx;
    packet->message.locy = me->locy;
    LOGD("sending discovery packet %d  %d at location (%d,%d)\n",packet->message.sendersInboxId,sizeof(packet->message)-sizeof(long),packet->message.locx,packet->message.locy);

}

void print_networkinfo(Tower * me)
{
    printf("\n");
    LOGI("My Inbox (%d) Friend's \n",me->myInboxId)
    for(unsigned int i=0; i<me->algoinfo.neighbourInboxId.size(); i++)
        LOGI("%d\n", me->algoinfo.neighbourInboxId[i]);
    printf("\n\n");
}

void sendpacket(message_buf sbuf,int rxid)
{

    if(msgsnd(rxid,&sbuf,sizeof(sbuf)-sizeof(long),IPC_NOWAIT)<0)
    {

        LOGD("message sending failed :-( %d,%ld %d %d \n",rxid,sbuf.messagetype,sizeof(sbuf)-sizeof(long),errno);
        perror("msgsnd");
    }
    else
        LOGI("Message sent to (%d) inbox from (%d)!. Im at (%d,%d) :-) \n",rxid,sbuf.message.sendersInboxId,sbuf.message.locx,sbuf.message.locy)
    }


void readmsg(message_buf* packet, Tower* me)
{
    switch(packet->messagetype)
    {
    case DISCOVERY:
    {

        int dis = abs(packet->message.locx - me->locx) + abs(packet->message.locy - me->locy);
        LOGI("Discovery packet recieved from (%d) inbox to (%d)!. He is at (%d,%d) :-) \n",packet->message.sendersInboxId, me->myInboxId, packet->message.locx,packet->message.locy);

        //mahattan distance <= 10 neighbours
        if(dis<=10 )
        {
            me->algoinfo.neighbourInboxId.push_back(packet->message.sendersInboxId);
            if(packet->message.ack!=0)
            {
                LOGI("Ack packet dont respond, just update tables :) \n")
                break;
            }
            /*making return discovery ack packet*/
            message_buf returnpacket;

            returnpacket.messagetype = DISCOVERY;
            returnpacket.message.ack = 1;
            returnpacket.message.locx = me->locx;
            returnpacket.message.locy = me->locy;
            returnpacket.message.sendersInboxId = me->myInboxId;
            LOGI("Sending ACK packet, He is a friend :)\n")
            sendpacket(returnpacket,packet->message.sendersInboxId);
        }

        break;
    }

    default:
        LOGD("Packet type unrecognized\n");

    }


}

/* Create a message q
while(1)
Read packet & do something
*/
void Tower_begin_function(int cnt,Tower* me)
{
    int msqid = 0;

    assert(towercount>0);


    if ((msqid = msgget(msgid[towercount-1], 0644)) < 0)
    {
        perror("msgget");
        assert(0);
    }
    message_buf rbuf;
    rbuf.messagetype = -1;
    /*poll for sometime ...  look for better alternative*/
    LOGI("Wait for 5secs Let others read and respond :-)")
    sleep(5);

    while(1)
    {
        if(msgrcv(msqid,&rbuf,sizeof(rbuf)-sizeof(long),0,IPC_NOWAIT)<0)
        {
            if(errno == ENOMSG)
            {
                //LOGD("No message recieved \n")
                break;
            }
            else
            {
                perror("msgrcv problem");
                assert(0);
            }

        }
        else
        {
            readmsg(&rbuf,me);

        }

    }

    LOGI("Unreliable may not have captured all friends ...")
    print_networkinfo(me);

    /*stability signal to parent only*/
    kill(mainprocessid,SIGUSR1);

    LOGD("Tower running ... Reading mails ... :-) \n")
    while(1)
    {
        /*Dont wait just read whats there */
        if(msgrcv(msqid,&rbuf,sizeof(rbuf)-sizeof(long),0,IPC_NOWAIT)<0)
        {
            if(errno == ENOMSG)
            {
                //LOGD("No message recieved \n")

            }
            else
            {
                perror("msgrcv problem");
                assert(0);
            }

        }
        else
            readmsg(&rbuf,me);
    }
}

/*Send discovery message to all*/
void Tower_init(Tower* me)
{
    LOGI("Tower init ... Creating inbox for tower %d  and discovering neighbours\n",towercount-1);
    int msgflg = IPC_CREAT|0644;
    int msqid;
    message_buf sbuf;

    if((me->myInboxId = msgget(msgid[towercount-1],msgflg))<=0)
    {
        perror("msgget failed");
        LOGD("init failed :-( .... \n")
        assert(0);

    }
    else
        LOGD(" created .... msgid[towercount-1] %d , my message queue %d",msgid[towercount-1],me->myInboxId)




    msgflg = 0644;
    for(int i=0; i<towercount-1; i++)
    {

        if((msqid = msgget(msgid[i],msgflg))<=0)
        {
            perror("msgget failed");
            assert(0);
        }
        LOGD("(key,msqid) of friends (%d,%d) \n",msgid[i],msqid);
        makedscoverypacket(me,&sbuf);
        sendpacket(sbuf,msqid);
        //assert(0);
    }
    if(towercount==1)
    {
        LOGD("First tower so im not writing \n")
    }
    else
    {
        LOGD("written to all neighbours \n")
    }

}


/*Create tower process*/
void create_tower_process(Tower tower)
{

    /*create here*/
    pid_t cpid= fork();
    int cnt = 0;
    towerprocesses[towercount++] = cpid; /*This variable makes sense in parent process*/
    switch(cpid)
    {
    case 0:
    {
        LOGD("New tower process created with pid %ld\n",(long)getpid());
        Tower_init(&tower);
        Tower_begin_function(cnt,&tower); /*this is an*/
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
