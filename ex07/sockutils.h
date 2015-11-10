/*
 * sockutils.h
 *
 *  Created on: Oct 31, 2015
 *      Author: hoshiro
 */

#ifndef EX06_SOCKUTILS_H_
#define EX06_SOCKUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXDATASIZE 100

/**
 * Create a socket and verify error.
 */
int Socket(int family, int type, int flags) {
	int sockfd;
	if ((sockfd = socket(family, type, flags)) < 0) {
		perror("socket");
		exit(1);
	} else
		return sockfd;
}

/**
 * Call bind and verify error.
 */
void Bind(int fd, struct sockaddr *addr, socklen_t length) {
	if (bind(fd, addr, length) == -1) {
		perror("bind");
		exit(1);
	}
}

/**
 * Call listen and verify error.
 */
void Listen(int socket, int n) {
	if (listen(socket, n) == -1) {
		perror("listen");
		exit(1);
	}
}

/**
 * Call accept and verify error.
 */
int Accept(int socket, struct sockaddr *addr, socklen_t *lengthptr) {
	int connfd;
	if ((connfd = accept(socket, addr, lengthptr)) == -1) {
		perror("accept");
		exit(1);
	}
	return connfd;
}

/**
 * Call connect socket and verify error.
 */
void Connect(int socket, struct sockaddr *addr, socklen_t length) {
	if (connect(socket, addr, length) < 0) {
		perror("connect error");
		exit(1);
	}
}

/**
 *	Call close and verify error.
 */
void Close (int socket) {
	if (close(socket) == -1) {
		perror("close");
		exit(1);
	}
}

#endif /* EX06_SOCKUTILS_H_ */
