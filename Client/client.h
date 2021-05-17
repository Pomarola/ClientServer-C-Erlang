#ifndef CLIENT
#define CLIENT

#define MSG_LEN 1200

void error(char *msg);

void *sendMsg();

void shut_down (int signal);

#endif