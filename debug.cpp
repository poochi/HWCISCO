/**/

void print_networkinfo(Tower * me)
{
    printf("\n");
    LOGI("My Inbox (%d) Friend's \n",me->myInboxId)
    for(unsigned int i=0; i<MAX_TOWERS; i++)
        LOGI("%d\n", me->algoinfo.neighbours[i]);
    printf("\n\n");
}