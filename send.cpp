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

/*send broadcast packet and update ack list*/
/*send n.w pkt*/
/*send rel pkt*/
/*send join pkt*/

/*recv join pkt*/
/*recv join pkt*/
/*recv join pkt*/

/*Acknowledge / Fresh */
void sendTableUpdatepacket(message_nwt_buf* sbuf,int rxid)
{
	sbuf->messagetype = NWTABLE;
    if(msgsnd(rxid,sbuf,sizeof(message_nwt_buf)-sizeof(long),IPC_NOWAIT)<0)
    {
        LOGD("message sending failed :-( %d,%ld %d %d \n",rxid,
             sbuf->messagetype,sizeof(message_nwt_buf)-sizeof(long),errno);
        perror("msgsnd");
    }
    else {
        LOGI("Message sent to (%d) inbox from (%d)!\n",
             rxid,sbuf->nwt.sendersInboxId)
	}
}

void sendReliabilitypacket(message_rel_buf* sbuf,int rxid) {	
	sbuf->messagetype = NWREL;
 if(msgsnd(rxid,sbuf,sizeof(message_rel_buf)-sizeof(long),IPC_NOWAIT)<0)
    {
		LOGD("message sending failed :-( %d,%ld %d %d \n",rxid,
             sbuf->messagetype,sizeof(message_rel_buf)-sizeof(long),errno);
        perror("msgsnd");
    }
    else {
        LOGI("Message sent to (%d) inbox from (%d)!. \n",
             rxid,sbuf->rel.sendersInboxId)
	}
}

void sendJoinpacket(message_join_buf* sbuf,int rxid) {	
	sbuf->messagetype = JOIN;
 if(msgsnd(rxid,sbuf,sizeof(message_join_buf)-sizeof(long),IPC_NOWAIT)<0)
    {
		LOGD("message sending failed :-( %d,%ld %d %d \n",rxid,
             sbuf->messagetype,sizeof(message_join_buf)-sizeof(long),errno);
        perror("msgsnd");
    }
    else {
        LOGI("Message sent to (%d) inbox from (%d).:-) \n",
             rxid,sbuf->join.sendersInboxId)
	}
}

