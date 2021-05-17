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

  /* Recibimos peticion de nickname por parte del servidor */
  recv(sock, buf, sizeof(buf),0);
  printf("Recv:%s\n", buf);

  if(strcmp(fgets(buf, sizeof(buf), stdin), "/exit\n") == 0)
    salir = 1;
  send(sock, buf, sizeof(buf), 0);

  recv(sock, buf, sizeof(buf),0);
  printf("Recv:%s\n", buf);     // Respondemos que el nickname se puso correctamente o decimos hasta luego

  while (!salir) {
      if(strcmp(fgets(buf, sizeof(buf), stdin), "/exit\n") == 0) // ver como hacer para que siempre se haga recv hasta que se intente ingresar por teclado
        salir = 1;
      send(sock, buf, sizeof(buf), 0);

      recv(sock, buf, sizeof(buf),0);
      printf("Recv:%s\n", buf);
  }

  /* Cerramos :D!*/
  freeaddrinfo(resultado);
  close(sock);

  return 0;
}
