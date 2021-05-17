ARG = -Wall -Wextra -Werror -g -std=gnu99 -pthread
SERVER = ./Server/server
CLIENT = ./Client/client
TEMP = server client

all: server client

server: $(SERVER).c $(SERVER).h
	gcc $(ARG) -o server $(SERVER).c -lm

client: $(CLIENT).c $(CLIENT).h
	gcc $(ARG) -o client $(CLIENT).c -lm

clean:
	-rm $(TEMP)

.PHONY : all
.PHONY : clean