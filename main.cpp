//Base station / cellular communication protocol/ statistics collection
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include "tower.h"
using namespace std;
extern void check_message_recieved(int src, int dest);
extern void init_towers();
extern void deinit_towers();
extern void send(int from, int to, int freq, void* message);
extern void print_towerdata() ;
extern void send(int from, int to, int freq, void* message);
extern void senddata(int src, int dest, char message[]) ;
extern void add_tower_to_network(Tower t, int index);
extern void remove_tower_from_network(int index);
extern void recieve(int to,int freq, void* message);

/*Simulator*/
int main() {

FILE* towersdata = fopen("tower.txt","r");
Tower temptower;

assert(towersdata!=0);
/*read towers in network*/
while(!feof(towersdata)) {
char temp[200];
fscanf(towersdata,"%s",temp);

int s = strlen(temp);
int i;
for(i=0;i<s;i++)
	if(temp[i]==':') 
		break;
temp[i++]='\0';
int index = atoi(&temp[1]);
int cnt = 0,pre;
index--;
assert(index<MAX_TOWERS);

for(pre=i+1,cnt=0;i<s;i++) {
//cout<<temp[i]<<endl;
	if(temp[i]==',') {
		temp[i++]='\0';
		int v = atoi(&temp[pre]);
		temptower.f[cnt++] = v;
		//T[index].f[cnt++]=v;
		assert(v<=FREQ_VALUE);
		//cout<<temp[pre]<<" "<<v<<" "<<"Asdasd"<<" "<<T[index].f[cnt-1]<<endl;
		pre = i+1;
		assert(cnt<=MAX_FREQ);
	}
}

int v = atoi(&temp[pre]);
//T[index].f[cnt++]=v;
temptower.f[cnt++] = v;
//cout<<temp[pre]<<" "<<v<<" "<<"Asdasd"<<T[index].f[cnt-1]<<endl;
assert(v<=FREQ_VALUE);
//T[index].freqused = cnt;
temptower.freqused = cnt;
assert(cnt<=MAX_FREQ);
/*Add this guy to network*/
add_tower_to_network(temptower,index);
}

print_towerdata();
fclose(towersdata);

/*Tower operations*/
freopen("input.txt","r",stdin);

int n;
scanf("%d",&n);
while(n--) {
//command
char s[10];
int i,j;
char data[20];
scanf("%s",s);
if(!strcmp(s,"TOWERUP")) {
scanf("%d",&i);
i--;
}
if(!strcmp(s,"TOWERDOWN")) {

scanf("%d",&i);
i--;
cout<<s<<endl;
}

if(!strcmp(s,"SENDDATA")) {
scanf("%d",&i);
scanf("%d",&j);
i--,j--;
scanf("%s",data);
assert(strlen(data)<20);
cout<<s<<endl;
}
	
}





return 0;
}