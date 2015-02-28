#include <stdio.h>
#include <assert.h>
#include <vector>
#include <unistd.h>
using namespace std;

#define MAX_TOWERS 10

#define TOWER_RECIEVE 0
#define TOWER_SEND 1
#define TOWER_INACTIVE 2
#define TOWER_MALFUNCTION 4
#define MAX_FREQ 5

#define FREQ_VALUE 100


struct networkinfo {
vector<int> neighbourInboxId;
vector<int> neighbour_reliablity; //time weighted measure
};

/*Tower Objects*/
/*Property of the tower is updated based on Algorithm*/
struct Tower {
int locx,locy;
int state;
int f[MAX_FREQ];
int freqused;
networkinfo algoinfo; //privatised to other guy
int myInboxId; // message queue key

Tower (int st) {
assert(st == TOWER_RECIEVE || st==TOWER_SEND);
state = st;
}
Tower() {
}

};

//#define DEBUG

#define LOGI(x,...) {\
printf("%ld ::"x,(long)getpid(),##__VA_ARGS__); \
}

#ifdef DEBUG

#define LOGD(x,...) { \
printf("%ld ::"x,(long)getpid(),##__VA_ARGS__); \
}
#else
#define LOGD(x,...) { \
}


#endif
