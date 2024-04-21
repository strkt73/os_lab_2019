CC=gcc
CFLAGS=-pthread -I.

all: client server

client: client.c common.o
	$(CC) -o $@ $^ $(CFLAGS)

server: server.c common.o
	$(CC) -o $@ $^ $(CFLAGS)

common.o: common.c common.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f client server common.o
