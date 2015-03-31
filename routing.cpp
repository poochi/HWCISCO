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

/*utility.cpp*/
extern int readTableUpdatepacket(message_nwt_buf *rbuf,int msqid);
extern int readReliabilitypacket(message_rel_buf *rbuf,int msqid);
extern int readJoinpacket(message_join_buf *rbuf,int msqid);

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


double absolute(double x){
	return x>0?x:-x;
}

extern int extractmsgid(int id);

void networkoperations(Tower *me)
{

	assert(me->state == TOWER_JOINED);
	
	message_nwt_buf nwt;	
	message_join_buf join;
	LOGD("Performing Network Operations \n");
	
	readTableUpdatepacket(&nwt,me->myInboxId);
	if(nwt.messagetype == NWTABLE) {
		if(nwt.nwt.ack != 1) {
			if(me->lastrcvd[nwt.nwt.senderID]> extractmsgid(nwt.nwt.message_id)) {
				me->lastrcvd[nwt.nwt.senderID]  = extractmsgid(nwt.nwt.message_id);
				memcpy(me->algoinfo.routingtable,nwt.nwt.table,sizeof(nwt.nwt.table));
				unsigned char sndr = nwt.nwt.senderID;
				/*sending broadcast msg*/
				nwt.nwt.senderID = me->myID; 
				nwt.nwt.sendersInboxId = me->myInboxId;			
				for(unsigned char i=0;i<MAX_TOWERS;i++) {
					if(i != sndr && me->algoinfo.neighbours[i] !=0){
						sendTableUpdatepacket(&nwt,me->algoinfo.neighbours[i]);
					}						
				}
			}			
		}
	}
	
	
	readJoinpacket(&join,me->myInboxId);
	if(join.messagetype == JOIN) {
		if(join.join.ack == 0) {
			LOGD("Friends are darn late in responding .... idiots !!! chuck them \n");
		} else {
				int sndrid = join.join.sendersInboxId;
				/*sending broadcast msg*/
				join.join.senderID = me->myID; 
				join.join.sendersInboxId = me->myInboxId;	
				memcpy(join.join.table,me->algoinfo.routingtable,sizeof(join.join.table));
				sendJoinpacket(&join,sndrid);	
		}
		
	}
}


/*you may join different parts of the network*/
void mergetable(double rcvdtable[MAX_TOWERS][MAX_TOWERS],Tower* me,int senderID) {
	for(int i=0;i<MAX_TOWERS;i++)
		for(int j=0;j<MAX_TOWERS;j++){
			if(rcvdtable[i][j]!=0)
				me->algoinfo.routingtable[i][j] = rcvdtable[i][j];
		if(i>j)
			assert(me->algoinfo.routingtable[i][j] == me->algoinfo.routingtable[j][i]);
		}
		me->algoinfo.routingtable[me->myID][senderID] = me->algoinfo.routingtable[senderID][me->myID] = 1;
}