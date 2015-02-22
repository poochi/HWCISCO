/*Notes & Limitations at end*/
#include <stdio.h>
#include "share.h"
#include "tower.h"
#include <stdlib.h>
#include <string.h>

Tower T[MAX_TOWERS];
int currenttowers=0; //0 indexed

/*propogate_algo.cpp*/
extern void notify_controller(int CMD,INFORMATION* info);



bool check_message_recieved(int src, int dest) {
if(T[src].msgtype !=UNREACHABLE_TARGET && T[dest].msgtype !=UNREACHABLE_TARGET) {
	if(!strcmp(T[src].data,T[dest].data))
		return true;
	return false;
}
assert(T[dest].msgtype == UNREACHABLE_TARGET);
printf("DUDE ARE YOU SERIOUS CANT REACH SRC??");
assert(0);
}


/*Debug*/
void print_towerdata() {
for(int i=0;i<=currenttowers;i++) {
	printf("Tower %d --- %d \n",i,T[i].freqused);
	for(int j=0;j<T[i].freqused;j++)
		printf("%d \n",T[i].f[j]);
	printf("\n");
}

		
}


void senddata(int src, int dest, char message[]) {
void* payload =  malloc(MAXIMUM_MESSAGE_SIZE);
memcpy(message,payload,strlen(message));
/*To do*/
//notify_controller(SENDDATA,&src, &dest, payload);

}

/*Immediate vicinity not more than 10 towers we allocate memory for this only*/
void add_tower_to_network(Tower t, int index) {
memcpy(&t,&T[index],sizeof(Tower));
void* pers = malloc(MAX_CONTROLLER_MEMORY);
T[index].algoinfo = pers;

INFORMATION info;
info.pers = pers;
info.freq = &T[index].f[0];
info.freqused = &T[index].freqused;

notify_controller(ADDTOWER,&info);
}

/*this is silent no notification to controller happens here*/
void remove_tower_from_network(int index) {
free(T[index].algoinfo);
/*invalidate all data to junk*/
memset(&T[index],-1,sizeof(Tower));
}


void tower_malfunction(int index) {
assert(index<=MAX_TOWERS && index>=0);
/*To be done*/
}

void recieve(int to,int freq, void* message) {
assert(to<=MAX_TOWERS && to>=0);
/*check may be unnecessary in serial tx case
if(T[to].state != TOWER_SEND) {
	printf("NOT IN A POSITION TO RECIEVE\n");
	return;
}*/
int type;

INFORMATION info;
info.pers = T[to].algoinfo;
info.message = message;
info.from = &to;

notify_controller(MSG_RECIEVED,&info);
}

// send a broadcast message to all Nearby towers 
void send_broadcast(void* message, int freq) {
for(int i=0;i<MAX_TOWERS;i++){
	recieve(i,freq,message);
}
}



//message is heap memory
void send(int from, int to, int freq, void* message) {
assert(from!=to );
assert(from<=MAX_TOWERS && to<=MAX_TOWERS);
assert(from>=0 && to>=0);

//check state of tower
if(T[to].state != TOWER_SEND && T[to].state != TOWER_RECIEVE){
	return; //silently cut communication
}
if(T[from].state != TOWER_SEND && T[from].state != TOWER_RECIEVE){
	printf("SHOULD NOT COME HERE WHY IS THE SENDING TOWER MALFUNCTIONING ????");
	assert(0);
	return; //silently cut communication
}
T[to].state = TOWER_RECIEVE;
T[from].state = TOWER_SEND;


//noise less channel debug
int i;
for( i=0;i<T[from].freqused && T[from].f[i] != freq;i++);
assert(i<T[from].freqused);

for( i=0;i<T[to].freqused && T[to].f[i] != freq;i++);
assert(i<T[to].freqused);

// edit here for lossy channel
recieve(to,freq,message);
}


/*Severe limitation
Serial sender algorithms only ---> Send, wait till recieve  happens then send again 
*/