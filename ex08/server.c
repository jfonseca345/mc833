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
 * Exec cmd and send output to client (This is will be used in next activity)
 */
void exec_cmd(char *cmd, int sock) {
	char output[MAXOUTPUTSIZE] = { 0 };
	FILE *fp;

	/** Discard new line stuff, for telnet reason.**/
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
//	cmd[strlen(cmd) - 2] = 0; // discard new line, for telnet tests.
	write(sock, cmd, strlen(cmd));
}

int main(int argc, char **argv) {
	int listenfd; // socket descriptor to listen to connection requests.
	int connfd; // socket descriptor to connect to client and send data.
	struct sockaddr_in servaddr; // struct that will contain server address info.
	char cmd[MAXDATASIZE]; // buffer that will contain data to be send to client.
	int port;

	struct sockaddr_in client_addr; // struct that will contain remote address info.
	socklen_t client_len; // addr size

	pid_t pid;

	if (argc < 2) {
		printf("Usage:\n\t%s <listen-port>\n", argv[0]);
		exit(1);
	}

	port = atoi(argv[1]);

	/** Create a ipv4 TCP socket that will be used to listen connections.
	 *  Also verify return for error checking. **/
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr)); // initialize struct with zeros.
	servaddr.sin_family = AF_INET; // set address family to IPV4.
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // set address to ANY. Convertions from host to network representation needed.
	servaddr.sin_port = htons(port); // set port. Convertion from host to network representation neeeded.

	/** Bind the socket described by listenfd to address created before.
	 *  Also verify return for error checking. **/
	Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	/** Marks the socket listenfd as a passsive socket that will respond
	 *  to connection requests and will have a queue with a maximum length
	 *  of LISTENQ. **/
	Listen(listenfd, LISTENQ);

	/** loops forever. **/
	for (;;) {
		/** Extract the first pending connection request on listenfd queue,
		 *  creates a new socket and return a new file descriptor for it.
		 *  Also verify return for error checking. **/
		connfd = Accept(listenfd, (struct sockaddr *) &client_addr, &client_len);

		/** create child to answer **/
		if ((pid = fork()) == 0) {
			close(listenfd);

			printf("=============================\n");
			printf("Session started.\n");
			printf("Client host info:\n");
			printf("%s : %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

			bzero(cmd, sizeof(cmd));
			while (read(connfd, cmd, sizeof(cmd))) {
				exec_and_echo(cmd, connfd);
				bzero(cmd, sizeof(cmd));
			}

			close(connfd);
			printf("Session ended.\n");
			printf("=============================\n");
			exit(0);
		}


		/** close socket connfd**/
		close(connfd);
	}
	return (0);
}
