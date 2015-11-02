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

#include "sockutils.h"

#define LISTENQ 10
#define MAXOUTPUTSIZE 200

/**
 * Exec cmd and send output to client
 */
void exec_cmd(char *cmd, int sock) {
	char output[MAXOUTPUTSIZE] = { 0 };
	FILE *fp;

	/** Discart new line stuff, for telnet reason.**/
	cmd[strlen(cmd) - 2] = 0;
	strcat(cmd, " 2>&1");

	fp = popen(cmd, "r");

	while (fgets(output, sizeof(output) - 1, fp) != NULL) {
		write(sock, output, strlen(output));
	}

	pclose(fp);

}

/**
 * Exec cmd and send output to client
 */
void exec_and_echo(char *cmd, int sock) {
//	cmd[strlen(cmd) - 2] = 0;
	system(cmd);
	write(sock, cmd, strlen(cmd));
}

int main(int argc, char **argv) {
	int listenfd; // socket descriptor to listen to connection requests.
	int connfd; // socket descriptor to connect to client and send data.
	struct sockaddr_in servaddr; // struct that will contain server address info.
	char cmd[MAXDATASIZE]; // buffer that will cotain data to be send to client.
	int port;

	struct sockaddr_in addr; // struct that will contain remote address info.
	socklen_t len; // addr size
	char ipstr[INET6_ADDRSTRLEN];

	pid_t pid;

	if (argc < 2) {
		printf("Usage:\n\t%s <listen-port>\n", argv[0]);
		exit(1);
	}

	port = atoi(argv[1]);

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

		/** creat child to answer **/
		if ((pid = fork()) == 0) {
			close(listenfd);

			bzero(cmd, sizeof(cmd));
			while (read(connfd, cmd, sizeof(cmd))) {
				exec_and_echo(cmd, connfd);
				bzero(cmd, sizeof(cmd));
			}
//			read(connfd, cmd, sizeof(cmd));

			close(connfd);
			exit(0);
		}

		/** get address info of socket connfd. **/
		getpeername(connfd, (struct sockaddr *) &addr, &len);
		/** convert read address from binary to string. **/
		inet_ntop(AF_INET, &addr.sin_addr, ipstr, sizeof(ipstr));
		/** print socket address info **/
//		printf("%s : %d\n", ipstr, ntohs(addr.sin_port));

//		/** close socket connfd**/
		close(connfd);
	}
	return (0);
}
