all:client server
.PHONY : all

cc = gcc

client : src/client.c
	$(cc) src/client.c -o bin/client
server : src/server.c
	$(cc) src/server.c -o bin/server -lsqlite3

.PHONY : clean
clean:
	rm bin/client bin/server
