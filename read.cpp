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

/*recv join pkt*/
/*recv join pkt*/
/*recv join pkt*/

/*To do*/
/*check and discard message based on timestamp*/

int internalhash(int msgid, int type, int origin) {
return msgid<<8 | type<<5 | origin ;
}

int readTableUpdatepacket(message_nwt_buf *rbuf,int msqid) {

        rbuf->messagetype = NO_MSG_RCVD;

        if(msgrcv(msqid,rbuf,sizeof(message_nwt_buf)-sizeof(long),NWTABLE,IPC_NOWAIT)<0)
        {
            if(errno == ENOMSG)
            {
                return 0;
            }
            else
            {
                perror("msgrcv problem");
                exit(1);
            }

        }
		return 1 ;
}

int readReliabilitypacket(message_rel_buf *rbuf,int msqid) {

        rbuf->messagetype = NO_MSG_RCVD;

        if(msgrcv(msqid,rbuf,sizeof(message_rel_buf)-sizeof(long),NWREL,IPC_NOWAIT)<0)
        {
            if(errno == ENOMSG)
            {

                return 0;
            }
            else
            {
                perror("msgrcv problem");
                exit(1);
            }

        }
		return 1 ;
}
int readJoinpacket(message_join_buf *rbuf,int msqid) {

        rbuf->messagetype = NO_MSG_RCVD;

        if(msgrcv(msqid,rbuf,sizeof(message_join_buf)-sizeof(long),JOIN,IPC_NOWAIT)<0)
        {
            if(errno == ENOMSG)
            {
                return 0;
            }
            else
            {
                perror("msgrcv problem");
                exit(1);
            }

        }
		return 1 ;
}
