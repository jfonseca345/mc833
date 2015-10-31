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

#define LISTENQ 10
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
 *
 */
void Bind(int fd, struct sockaddr *addr, socklen_t length) {
	if (bind(fd, addr, length) == -1) {
		perror("bind");
		exit(1);
	}
}

/**
 *
 */
void Listen(int socket, int n) {
	if (listen(socket, LISTENQ) == -1) {
		perror("listen");
		exit(1);
	}
}

int Accept(int socket, struct sockaddr *addr, socklen_t *lengthptr) {
	int connfd;
	if ((connfd = accept(socket, addr, lengthptr)) == -1) {
		perror("accept");
		exit(1);
	}
	return connfd;
}

int main(int argc, char **argv) {
	int listenfd; // socket descriptor to listen to connection requests.
	int connfd; // socket descriptor to connect to client and send data.
	struct sockaddr_in servaddr; // struct that will contain server address info.
	char buf[MAXDATASIZE]; // buffer that will cotain data to be send to client.
	time_t ticks;
	int port;

	struct sockaddr_in addr; // struct that will contain remote address info.
	socklen_t len; // addr size
	char ipstr[INET6_ADDRSTRLEN];

	if (argc < 2) {
		printf("Usage:\n\t%s <listen-port>\n", argv[0]);
		exit(1);
	}

	port = atoi(argv[0]);

	/** Create a ipv4 TCP socket that will be used to listen connections.
	 *  Also verify return for error checking. **/
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr)); // inialize struct to all zeros.
	servaddr.sin_family = AF_INET; // set address family to IPV4
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // set address to any. Convertions from host to network representation needed.
	servaddr.sin_port = htons(port); // set port. Convertion from host to network representation neeeded.

	/** Bind the socket described by listenfd to address created before.
	 *  Also verify return for error checking. **/
	Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	/** Marks the socket listenfd as a passsive socket that will respond
	 *  to connections request and will have a queue with a maximum length
	 *  of LISTENQ. **/
	Listen(listenfd, LISTENQ);

	/** loops forever. **/
	for (;;) {
		/** Extract the first pending connection request on listenfd queue,
		 *  creates a new socket and return a new file descriptor for it.
		 *  Also verify return for error checking. **/
		connfd = Accept(listenfd, (struct sockaddr *) NULL, NULL);

		/** creates a string containing current data. **/
		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
		/** buffer write content to socket. **/
		write(connfd, buf, strlen(buf));

		/** get address info of socket connfd. **/
		getpeername(connfd, (struct sockaddr *) &addr, &len);
		/** convert read address from binary to string. **/
		inet_ntop(AF_INET, &addr.sin_addr, ipstr, sizeof(ipstr));
		/** print socket address info **/
		printf("%s : %d\n", ipstr, ntohs(addr.sin_port));

		/** close socket connfd**/
		close(connfd);
	}
	return (0);
}
