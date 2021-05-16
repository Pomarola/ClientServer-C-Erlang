#ifndef SERVER
#define SERVER

#define MSG_LEN 1024
/* Maxima cantidad de cliente que soportará nuestro servidor */
#define MAX_CLIENTS 20

typedef struct _argsT {
    int sockClient;
    pthread_mutex_t mutex;
} argsT_t;

typedef struct _client {
    int socket = -1;
    char nickname[100];
} client;

#endif