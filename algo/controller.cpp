/*Algorithm */
#include <assert.h>
#include "share.h"
#include <stdio.h>

/*Network simulators*/
extern void send_broadcast(void* message, int freq);
extern void send(int from, int to, int freq, void* message);

#define TESTING_ONLY
#ifdef TESTING_ONLY
void makediscoverypackt(int from, int freq,void* message) {

}
void control_init(void* pers, int from,int freused,int freq[]){
}
void analyse_data(void* message, void* pers){
}
#endif


void networkdiscovery(int from, int freqused, int freq[], void* message) {
//make packet
//send broadcast message
printf("Making Network Discovery packet ... passing control to makediscoverypackt (Sreenivasa) \n");
for(int i=0;i<freqused;i++) {
	makediscoverypackt(from,freq[i],message);
//trust sreenivasa
	printf("Sending a broadcast message for NEWGUY advert Network Discovery packet ... passing control to networkSimulator (poochi) \n");
	send_broadcast(message,freq[i]);
}
}


/*CONTROLLER API*/
void notify_controller(int CMD,INFORMATION* info){
switch(CMD) {
case ADDTOWER:
	
	printf("Advertising New tower initiated \n");
	//notify_controller(ADDTOWER,pers, (void*) &(T[index].f), (void*)T[index].freqused );

	printf("Controlling init\n");
	control_init(info->pers,*(info->from),*(info->freqused),&(info->freq[0]));	
	networkdiscovery(*(info->from),*(info->freqused),&(info->freq[0]),info->message);
	break;

case TYPE_OF_MESSAGE_RECIEVED: {
	/*Do nothing*/
}
case MSG_RECIEVED: {
	assert(info->to!=0 && info->message!=0 && info->pers!=0);
	printf("Message recieved to %d ---- \n",*(info->to));
	analyse_data(info->message,info->pers);		
}
	
}
return;
}