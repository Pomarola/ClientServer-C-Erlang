# Ejercicio Semanal IPC

El ejercicio consta de la implementación de una sala de chat con la capacidad de
los usuarios puedan hablar entre ellos de forma instantánea. Deberán implementar
un servidor, y un cliente, tal que una vez establecida la conexión, los clientes
puedan conversar libremente.

A modo de ejemplo se pueden inspirar en lo que eran los servicios de 
[IRC](https://es.wikipedia.org/wiki/Internet_Relay_Chat).

## Detalles del Ejercicio

+ Fecha de entrega: Lunes 17 de Mayo a las 23.59 (GMT-3).
+ Escritura y lectura asíncrona.
+ Tanto los clientes como el servidor estarán en la misma red, y serán
  alcanzables.
  
## Detalles de la Implementación

+ Longitud máxima del mensaje: 1024 caracteres.
+ Compilación con `make`: se esperan dos ejecutables `cliente` y `servidor`.
+ El cliente tomará la dirección Ipv4 y puerto al momento de ejecutarlo de la
  siguiente forma:`$./cliente DirIp Puerto`.
+ El servidor tomará el puerto al cual esperará al cliente:`$./servidor Puerto`.
+ Asumimos que hay un solo salón donde se puede enviar mensajes de forma general
  a todo el mundo.
+ El cliente debe proveer un nombre (o *nickname*) que será único.
+ `/msg [nickname] [msg]` le enviará un el mensaje `msg` al usuario con nombre `nickname`.
+ `/exit` saldrá del cliente.
+ `/nickname [nickname]` intentará cambiar el nombre de usuario del cliente.

## Fin de la comunicación

Deberán finalizar la comunicación amablemente. Es decir, deberán establecer un
protocolo de finalización de comunicación entre ambos programas.

Una vez finalizada la conexión deberán liberar todos los recursos utilizado en
el programa, incluso cuando haya algún error en la interacción con el sistema
operativo.

NB: Para comenzar la correcta finalización de la conexión recomendamos utilizar
*señales*.
