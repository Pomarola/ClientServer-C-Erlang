#include "server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

client* init_client_array() {

  for (int i = 0; i < MAX_CLIENTS; i++) {
    clientArray[i].socket = -1;
  }

  return clientArray;
}

int search_nickname(client* clientArray, char* nickname) {
  int found = 0, i = 0;
  for (; i < MAX_CLIENTS && !found; i++) {
    if (!strcmp(clientArray[i]->nickname, nickname))
      found = 1;
  }

  return found;
}

// Devuelve 0 si no lo pudo eliminar
int delete_client (client* clientArray, int sockClient) {
  int found = 0, i = 0;
  for (; i < MAX_CLIENTS && !found; i++) {
    if (clientArray[i]->socket == sockClient)
      found = 1;
  }
  if (i != MAX_CLIENTS) {
    clientArray[i].socket = -1;
  }

  return found
}

/* Devuelve:
0 msg al canal general
1 msg privado
2 cambio nickname
3 exit
4 comando no reconocido
*/
int take_action (char* buf) {
  int action = 0;

  if (buf[0] == '/') {
    if (!strncmp(buf, "/msg ", strlen("/msg ")))
      action = 1;
    else if (!strncmp(buf, "/nickname ", strlen("/nickname ")))
      action = 2;
    else if (!strcmp(buf, "/exit"))
      action = 3;
    else
      action = 4;
  }

  return action;
}

int main(int argc, char **argv){
  int sock, *soclient;
  struct sockaddr_in servidor, clientedir;
  socklen_t clientelen;
  pthread_t thread;
  pthread_attr_t attr;

  if (argc <= 1) error("Faltan argumentos");

  /* Creamos el socket */
  if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    error("Socket Init");

  /* Creamos a la dirección del servidor.*/
  servidor.sin_family = AF_INET; /* Internet */
  servidor.sin_addr.s_addr = INADDR_ANY; /**/
  servidor.sin_port = htons(atoi(argv[1]));

  /* Inicializamos el socket */
  if (bind(sock, (struct sockaddr *) &servidor, sizeof(servidor)))
    error("Error en el bind");

  printf("Binding successful, and listening on %s\n",argv[1]);

  /************************************************************/
  /* Creamos los atributos para los hilos.*/
  pthread_attr_init(&attr);
  /* Hilos que no van a ser *joinables* */
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  /************************************************************/

  /* Ya podemos aceptar conexiones */
  if(listen(sock, MAX_CLIENTS) == -1)
    error(" Listen error ");

  pthread_mutex_t mutexLock = PTHREAD_MUTEX_INITIALIZER;

  for(;;){ /* Comenzamos con el bucle infinito*/
    /* Pedimos memoria para el socket */
    soclient = malloc(sizeof(int));

    /* Now we can accept connections as they come*/
    clientelen = sizeof(clientedir);
    if ((*soclient = accept(sock
                          , (struct sockaddr *) &clientedir
                          , &clientelen)) == -1)
      error("No se puedo aceptar la conexión. ");

    argsT_t argsT;

    argsT.mutex = mutexLock;
    argsT.sockclient = soclient;
    /* Le enviamos el socket al hijo*/
    pthread_create(&thread , &attr , worker, (void *) &argsT);

    /* El servidor puede hacer alguna tarea más o simplemente volver a esperar*/
  }

  /* Código muerto */
  close(sock);

  return 0;
}

void * worker(void* argsT){
  pthread_mutex_t mutexLock = ((argsT_t*)argsT)->mutex;
  int soclient = ((argsT_t*)argsT)->sockclient;
  int working = 0;
  char buf[MSG_LEN];

  /* SEND PING! */
  send(soclient, "PING!", sizeof("PING!"), 0);
  /* WAIT FOR PONG! */
  recv(soclient, buf, sizeof(buf), 0);
  printf("Hilo[%ld] --> Recv: %s\n", pthread_self(), buf);

  while (!working) {
    int action = take_action(buf);
    switch (action) {
    case 0:
      // Envia a chat general
      break;

    case 1:
      // msg privado
      break;

    case 2:
      // cambio nickname
      break;

    case 3:
      strcpy(buf, "Hasta luego!");
      working = 1;
      break;
    
    default:
      strcpy(buf, "Comando no reconocido");
      break;
    }

    send(soclient, buf, sizeof(buf), 0);
  }

  free(&soclient);
  return NULL;
}

void error(char *msg){
  exit((perror(msg), 1));
}
