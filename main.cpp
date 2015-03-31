//Base station / cellular communication protocol/ statistics collection
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

extern void create_tower_process(Tower tower);

void Tower_reaper(int signal);
void cleanup(int signal);

pid_t towerprocesses[MAX_TOWERS];
key_t msgid[MAX_TOWERS];

int towercount = 0;
pid_t mainprocessid = 0;


int max(int x, int y)
{
    return x>y?x:y;
}

/*Tower process sends Im stable signal*/
static void stability(int signal)
{
    LOGD("Stability signal recieved \n")
}
static void close_message_queues()
{

    int msqid;
    LOGD("closing message queues\n");
    for(int i=0; i<towercount; i++)
    {
        if ((msqid = msgget(msgid[i], 0644)) < 0)
        {
            perror("msgget");
            exit(1);
        }
        if(msgctl(msqid, IPC_RMID, NULL)<0)
        {
            perror("msgctl");
        }
    }

}

/*Read Input*/
int main()
{

    Tower temptower[MAX_TOWERS];
    memset(temptower,0,sizeof(Tower));
    int maxtowers = 0;

    /*this signal is risen when the network reaches stability after
    every operation*/
    signal (SIGUSR1, stability);

    assert(towersdata!=0);

    mainprocessid = getpid();
    signal (SIGCHLD, Tower_reaper);
    signal(SIGINT, cleanup);


    /*from stdin*/
    freopen("testinput/input.txt","r",stdin);
    scanf("%d",&maxtowers);
    for(int i=0,x,y; i<maxtowers; i++)
    {
        scanf("%d %d",&x,&y);
        assert(x>=0 && y>=0);
        temptower[i].locx = x;
        temptower[i].locy = y;
    }

    /*create and tower processes one by one*/
    for(int i=0; i<maxtowers; i++)
    {
        /*Create message queue for each tower process*/    
        msgid[i] = (getpid()+ i*39)%10000;
        LOGD("Locations of towers :: %d %d \n",temptower[i].locx,
             temptower[i].locy)
        create_tower_process(temptower[i]);
        /*Waiting for Stability */
        pause(); 
    }

    LOGI("Sleeping for 100s time  use cntrl+c to quit :-) \n")
    sleep(100);
    LOGD("Woke up Killing all my children\n")
    cleanup(0);

    return 0;
}

void Tower_reaper(int signal)
{
    int state;
    LOGD(" SIG CHLD Collecting Zombies should be in parent process \n");
    assert(getpid()== mainprocessid);
    pid_t pid;
    /*reap one child*/
    while((pid = waitpid(-1,&state, WNOHANG|WUNTRACED))<0);
    LOGD("state of exited process -- %d \n",state)
}

/*kill all processes if its parent and close parent too  */
void cleanup(int signal)
{
    if(mainprocessid == getpid())
    {
        /*close my message queue and return out*/
        close_message_queues();
        for(int i=0; i<towercount; i++)
            kill(towerprocesses[i],SIGTERM);
        LOGD("Killed \n");

    }
    else
    {
        LOGD("Sending interrupt to main process \n")
        kill(mainprocessid,SIGINT);

    }
    exit(0);
}
