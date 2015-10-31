CC=gcc
CFLAGS=-Wall -g
DEPS=myconn_err.h
SRCDIR=ex06/


all: server client

server: $(SRCDIR)/server.c
	$(CC) $(CFLAGS) $(SRCDIR)server.c -o server

client: $(SRCDIR)/client.c
	$(CC) $(CFLAGS) $(SRCDIR)client.c -o client

clean:
	rm server client
