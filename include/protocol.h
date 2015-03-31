#ifndef PROTOCOL_H
#define PROTOCOL_H


#define AVGRTT_PER_TOWER 10

/*NJOIN*/
struct  message_join_buf
{
    long messagetype;
    struct msg
    {
        char ack; // to have read reciept
        int message_id; // hash counter for
        int locx;
        int locy;
        int sendersInboxId;        
        double table[MAX_TOWERS][MAX_TOWERS];
        unsigned char senderID;
        unsigned char OriginID;
	} join;
};

/*NWT*/
struct  message_nwt_buf
{
    long messagetype;
    struct msg
    {
        char ack; // to have read reciept
        int message_id; // is for a specific origin tower
        int sendersInboxId;        
        double table[MAX_TOWERS][MAX_TOWERS]; /*entire table*/
        unsigned char senderID;
    } nwt;

};


/*REL*/
struct  message_rel_buf
{
    long messagetype;
    struct msg
    {
        char ack; // to have read reciept
        int message_id; // is for a specific origin tower and msgtype
        int sendersInboxId;        
        double reliability[MAX_TOWERS];
        unsigned char senderID;
    } rel;

};


#define MAXPROTOCOLSIZE max(sizeof(message_rel_buf), max(sizeof(message_nwt_buf), sizeof(message_join_buf)))
#endif
