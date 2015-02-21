
/*all sizes are in bytes*/

#define UNREACHABLE_TARGET -1

#define MAXIMUM_MESSAGE_SIZE  1024
#define MAX_CONTROLLER_MEMORY  1024

/*API communicated to controller*/
#define DEINIT						0
#define INIT						1
#define TYPE_OF_MESSAGE_RECIEVED	2
#define PREPARE_TO_SEND				3
#define SENDDATA					4
#define ADDTOWER					5
#define MSG_RECIEVED				6