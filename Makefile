CC=gcc
CFLAGS=-Wall -g
DEPS=myconn_err.h
SRCDIR=ex07/
HEADERS=$(SRCDIR)/*.h

all: server client

server: $(SRCDIR)/server.c $(HEADERS)
	$(CC) $(CFLAGS) $(SRCDIR)server.c -o server

client: $(SRCDIR)/client.c $(HEADERS)
	$(CC) $(CFLAGS) $(SRCDIR)client.c -o client

clean:
	rm server client
