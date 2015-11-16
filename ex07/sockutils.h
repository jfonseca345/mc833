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
#include <signal.h>
#include <sys/wait.h>
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
void Close(int socket) {
	if (close(socket) == -1) {
		perror("close");
		exit(1);
	}
}

/**
 *
 */
typedef void Sigfunc(int);
Sigfunc * Signal(int signo, Sigfunc *func) {
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask); /* Outros sinais não são bloqueados*/
	act.sa_flags = 0;
	if (signo == SIGALRM) { /* Para reiniciar chamadas interrompidas */
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
#endif
	}
	if (sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	return (oact.sa_handler);
}

/**
 *
 */
void sig_chld(int signo) {
	pid_t pid;
	int stat;
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

/**
 *
 */
void err_sys(const char* x) {
	perror(x);
	exit(1);
}

#endif /* EX06_SOCKUTILS_H_ */
