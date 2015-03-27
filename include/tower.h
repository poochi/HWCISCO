#include <stdio.h>
#include <assert.h>
#include <vector>
#include <unistd.h>
#include <>
using namespace std;

#define MAX_TOWERS              10
#define MAX_MESSAGEID           8888

/*Internal tower state :- Algorithm based*/
#define TOWER_PREJOIN		0
#define TOWER_JOINED		1

struct networkinfo {
/*contains reliability*/
double routingtable[MAX_TOWERS][MAX_TOWERS]; 
/*should not be used except for initial broadcast*/
int alltowers[MAX_TOWERS];
};


struct Tower {
	int locx,locy;
	int state;
	networkinfo algoinfo; 
	sqlite3 *messagedb;
	/*message queue key*/
	int myInboxId;
	int myId; 
	int message_no;
};



#define DEBUG

#define LOGI(x,...) {\
printf("%ld ::"x,(long)getpid(),##__VA_ARGS__); \
}

#ifdef DEBUG

#define LOGD(x,...) { \
char logds[20]; \
sprintf(logds,"%lu", (long)getpid()); \
FILE* logdfp = fopen(logds,"a+"); \
fprintf(logdfp,"%ld ::"x,(long)getpid(),##__VA_ARGS__); \
fclose(logdfp); \
}
#else
#define LOGD(x,...) { \
}


#endif
