CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -pthread

.PHONY: all tcp udp clean

all: tcp udp

tcp: tcpclient tcpserver

udp: udpclient udpserver

tcpclient: tcpclient.c
	$(CC) $(CFLAGS) -o $@ $<

tcpserver: tcpserver.c
	$(CC) $(CFLAGS) -o $@ $<

udpclient: udpclient.c
	$(CC) $(CFLAGS) -o $@ $<

udpserver: udpserver.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f tcpclient tcpserver udpclient udpserver
