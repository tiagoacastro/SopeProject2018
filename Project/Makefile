all: make

make: start client server

start: start.c
	gcc -Wall start.c -o start

client: client.c
	gcc -Wall client.c -o client

server: server.c
	gcc -D_REENTRANT -pthread -Wall server.c -o server

clean:
	rm -rf start client server
