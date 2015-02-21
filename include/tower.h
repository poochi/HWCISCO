#include <stdio.h>
#include <assert.h>

#define MAX_TOWERS 100

#define TOWER_RECIEVE 0
#define TOWER_SEND 1
#define TOWER_INACTIVE 2
#define TOWER_MALFUNCTION 4
#define MAX_FREQ 5

#define FREQ_VALUE 100 
//Just for sanity checking input parsing

/*Tower Objects*/
/*Property of the tower is updated based on Algorithm*/
struct Tower {
int state;
int f[MAX_FREQ]; 
int freqused;
void* algoinfo; //privatised to other guy
int msgtype;
char data[10];

Tower (int st) {
assert(st == TOWER_RECIEVE || st==TOWER_SEND);
state = st;
}
Tower() {
}

};