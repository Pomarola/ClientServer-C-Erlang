#include "client.h"

/* RemoteClient.c
   Se introducen las primitivas necesarias para establecer una conexión simple
   dentro del lenguaje C utilizando sockets.
*/
/* Cabeceras de Sockets */
#include <sys/types.h>
#include <sys/socket.h>
/* Cabecera de direcciones por red */
#include <netdb.h>
/**********/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

/*
  El archivo describe un sencillo cliente que se conecta al servidor establecido
  en el archivo RemoteServer.c. Se utiliza de la siguiente manera:
  $cliente IP port
 */

void error(char *msg){
  exit((perror(msg), 1));
}

int main(int argc, char **argv){
  int sock, salir = 0;
  char buf[1024];
  struct addrinfo *resultado;

  /*Chequeamos mínimamente que los argumentos fueron pasados*/
  if(argc != 3){
    fprintf(stderr,"El uso es \'%s IP port\'", argv[0]);
    exit(1);
  }

  /* Inicializamos el socket */
  if( (sock = socket(AF_INET , SOCK_STREAM, 0)) < 0 )
    error("No se pudo iniciar el socket");

  /* Buscamos la dirección del hostname:port */
  if (getaddrinfo(argv[1], argv[2], NULL, &resultado)){
    fprintf(stderr,"No se encontro el host: %s \n",argv[1]);
    exit(2);
  }

  if(connect(sock, (struct sockaddr *) resultado->ai_addr, resultado->ai_addrlen) != 0)
    /* if(connect(sock, (struct sockaddr *) &servidor, sizeof(servidor)) != 0) */
    error("No se pudo conectar :(. ");

  printf("La conexión fue un éxito!\n");

  pthread_t thread;
  int connected = 1;
  pthread_create(&thread, NULL, sendMsg, (void*) sock);

  while(connected) {
    recv(sock, buf, sizeof(buf),0);
    printf("%s\n", buf);
    if(!strcmp(buf, "Hasta luego!"))
      connected = 0;
  }
  /* Cerramos :D!*/
  freeaddrinfo(resultado);
  close(sock);

  return 0;
}

void * sendMsg(void* arg) {
  int socketSv = *(int*) arg;
  int conected = 1;
  char buf[MSG_LEN];

  while (conected) {
    if(!strcmp(fgets(buf, sizeof(buf), stdin), "/exit\n"))
      conected = 0;
    send(socketSv, buf, sizeof(buf), 0);
  }

  return NULL;
}
