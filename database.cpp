/*Database interface simple*/

/*
Currently Dummy; To be used for Ack checks etc., 
*/

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

#include "include/tower.h"


static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   //fprintf(stderr, "%s: ", (const char*)data);
   
   for(i=0; i<argc; i++){
      LOGD("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
 return 0;
}


void updatePendingAck(Tower* me, int message_id, void *pkt,int type, int sender, int size) {
    char insert_sql[1024];
    sqlite3_stmt *insert_stmt;
    sqlite3_blob *blob;
    int rc = 0;

    sprintf(insert_sql,"INSERT INTO MESSAGE (message_id, msgtype, msg, senderInbox) VALUES('%d', '%d' , ? , '%d');"
        ,message_id,type,sender);
      rc = sqlite3_prepare_v2(me->messagedb, insert_sql, -1, &insert_stmt, NULL);
   if(SQLITE_OK != rc) {
      fprintf(stderr, "Can't prepare insert statment %s (%i): %s\n", insert_sql, rc, sqlite3_errmsg(me->messagedb));
      sqlite3_close(me->messagedb);
      exit(1);
   }

   // Bind a block of zeros the size of the file we're going to insert later
   sqlite3_bind_zeroblob(insert_stmt, 1, size);
   if(SQLITE_DONE != (rc = sqlite3_step(insert_stmt))) {
      fprintf(stderr, "Insert statement didn't work (%i): %s\n", rc, sqlite3_errmsg(me->messagedb));
      exit(1);
   }

   sqlite3_int64 rowid = sqlite3_last_insert_rowid(me->messagedb);     
   
   rc = sqlite3_blob_open(me->messagedb, "main", "MESSAGE" , "msg", rowid, 1, &blob);
   if(SQLITE_OK != rc) {
      fprintf(stderr, "Couldn't get blob handle (%i): %s\n", rc, sqlite3_errmsg(me->messagedb));
      exit(1);
   }
   if(SQLITE_OK != (rc = sqlite3_blob_write(blob, pkt, size, 0))) {
        fprintf(stderr, "Error writing to blob handle. Offset %i, len %i\n", 0, size);
        exit(1);
   }
   sqlite3_finalize(insert_stmt);
   sqlite3_blob_close(blob);

 }


 void opendb(Tower* me) {
	char a[100];
  char *zErrMsg = 0;
    sprintf(a,"%ld.msgdb",(long)getpid());
    a[99] = 0;
    int rt = sqlite3_open(a, &me->messagedb);
    if(rt) {
        LOGI("Unable to Open me->messagedb file\n");
        sqlite3_close(me->messagedb);
        exit(0);
    }
    /*MSGTYPE,MSG,RESPONSE,TIMETOCLEAR*/
    sprintf(a,"CREATE TABLE MESSAGE (message_id int, msgtype int, msg BLOB, senderID int);");
    rt = sqlite3_exec(me->messagedb, a
        , callback, 0, &zErrMsg);
    if( rt!=SQLITE_OK ){
      LOGI("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
}
 
 void closedb(Tower* me) {
	sqlite3_close(me->messagedb);
}

