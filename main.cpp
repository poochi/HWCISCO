//Base station / cellular communication protocol/ statistics collection
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/time.h>
#include <sys/wait.h>
#include "tower.h"

extern void create_tower_process(Tower tower);

void Tower_reaper(int signal);
void kill_all_towers(int signal);

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

/*Read Input*/
int main()
{

    FILE* towersdata = fopen("testinput/tower.txt","r");
    Tower temptower[MAX_TOWERS];
    memset(temptower,0,sizeof(Tower));
    int maxtowers = 0;
    assert(towersdata!=0);

    /*this signal is risen when the network reaches stability after every operation*/
    signal (SIGUSR1, stability);

    assert(towersdata!=0);

    /*read towers in network*/
    while(!feof(towersdata))
    {
        char temp[200];
        fscanf(towersdata,"%s",temp);

        int s = strlen(temp);
        int i;
        for(i=0; i<s; i++)
            if(temp[i]==':')
                break;
        temp[i++]='\0';
        int index = atoi(&temp[1]);
        int cnt = 0,pre;
        index--;
        assert(index<MAX_TOWERS);

        for(pre=i+1,cnt=0; i<s; i++)
        {
            //cout<<temp[i]<<endl;
            if(temp[i]==',')
            {
                temp[i++]='\0';
                int v = atoi(&temp[pre]);
                temptower[index].f[cnt++] = v;
                assert(v<=FREQ_VALUE);
                assert(cnt<=MAX_FREQ);
                pre = i+1;

            }
        }

        int v = atoi(&temp[pre]);
        temptower[index].f[cnt++] = v;
        temptower[index].freqused = cnt;

        assert(cnt<=MAX_FREQ);
        assert(v<=FREQ_VALUE);
        maxtowers = max(maxtowers,index);
    }
    fclose(towersdata);

    mainprocessid = getpid();
    signal (SIGCHLD, Tower_reaper);
    signal(SIGINT, kill_all_towers);


    /*from stdin*/
    freopen("testinput/input.txt","r",stdin);

    for(int i=0,x,y; i<=maxtowers; i++)
    {
        scanf("%d %d",&x,&y);
        assert(x>=0 && y>=0);
        temptower[i].locx = x;
        temptower[i].locy = y;
    }

    /*create and tower processes one by one*/
    for(int i=0; i<=maxtowers; i++)
    {
        /*key is to create message queue for each tower process*/
        /*should clean up message queue also TODO*/
        msgid[i] = (getpid()+ i*39)%10000;
        LOGD("Locations of towers :: %d %d \n",temptower[i].locx,temptower[i].locy)
        create_tower_process(temptower[i]);
        pause(); //waiting for stability in network ; you need this ??
    }

    LOGI("Sleeping for 100s time  use cntrl+c to quit :-) \n")
    sleep(100);
    LOGD("Woke up Killing all my children\n")
    kill_all_towers(0);

    return 0;
}




void Tower_reaper(int signal)
{
    int state;
    LOGD(" SIG CHLD Collecting Zombies should be in parent process \n");
    assert(getpid()== mainprocessid);
    pid_t pid;
    while((pid = waitpid(-1,&state, WNOHANG|WUNTRACED))<0);
}

/*Kill all tower process*/
void kill_all_towers(int signal)
{
    LOGD("towercount %d \n",towercount)
    for(int i=0; i<towercount; i++)
    {
        LOGD("Killing .... %ld\n",(long)towerprocesses[i])
        kill(towerprocesses[i],SIGKILL);
    }
    LOGD("Killed \n")
    /*kill all their message queue*/
}


