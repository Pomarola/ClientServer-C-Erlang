#ifndef SERVER
#define SERVER

#define MSG_LEN 1024

typedef struct _argsT {
    int sockclient;
    pthread_mutex_t mutex;
} argsT_t;

#endif