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

double absolute(double x){
	return x>0?x:-x;
}



void networkoperations(Tower *me)
{
    assert(packet!=0 && me !=0);	
	assert(me->state == JOINED);
	
	message_nwt_buf nwt;
	message_rel_buf rel;
	message_join_buf join;
	LOGD("Performing Network Operations \n");
	
	readTableUpdatepacket(&nwt,me->myInboxID);
	if(nwt.messagetype == NWTABLE) {
		if(nwt.nwt.ack == 1)
			messagerepo(me,nwt->,REMOVE_PENDING_ACK);
		else{		
			if(messagerepo(me,,ISNEW)) {			
				memcpy(me->algoinfo.routingtable,nwt.nwt.table,sizeof(nwt.nwt.table));
				unsigned char sndr = nwt.nwt.SendersID;
				/*sending broadcast msg*/
				nwt.nwt.SendersID = me->myID; 
				nwt.nwt.SendersInbox = me->myInboxID;			
				for(unsigned char i=0;i<MAX_TOWERS;i++) {
					if(i != sndr && me->algoinfo.neighbours[i] !=0){
						sendTableUpdatepacket(&nwt,me->algoinfo.neighbours[i]);
					}						
				}
			}
			int sndrid = nwt.nwt.SendersInboxID;
			
			nwt.nwt.ack = 1;
			nwt.nwt.SendersID = me->myID; 
			nwt.nwt.SendersInbox = me->myInboxID;			
			sendTableUpdatepacket(&nwt,sndrid);
		}			
	}
	
	//readReliabilitypacket(&rel,me->myInboxID);
	
	readJoinpacket(&join,me->myInboxID);
	if(join.messagetype = JOIN) {
		if(join.join.ack ==0) {
			LOGD("Friends are darn late in respoding .... idiots !!! chuck them \n");
		} else {
				int sndr = join.join.SendersInboxID;
				/*sending broadcast msg*/
				join.join.SendersID = me->myID; 
				join.join.SendersInbox = me->myInboxID;	
				memcpy(join.join.table,me->algoinfo.routingtable,sizeof(join.join.table));
				sendJoinpacket(&join,sndrid);	
		}
		
	}
	
	/*Check pendingAck*/	
	messagerepo(CHECKANDSEND);
	/**/
	
	
}



