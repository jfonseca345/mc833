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
	char output_ln[MAXOUTPUTSIZE] = { 0 };
	char output[MAXOUTPUTSIZE * 10] = {0};
	FILE *fp;

	/** Discard new line stuff, for telnet reason.**/
//	cmd[strlen(cmd) - 2] = 0;
	/** redirect errors **/
	strcat(cmd, " 2>&1");

	fp = popen(cmd, "r");

	fgets(output_ln, sizeof(output_ln) - 1, fp);
	strcpy(output, output_ln);

	while (fgets(output_ln, sizeof(output_ln) - 1, fp) != NULL) {
		strcat(output, output_ln);
	}

	write(sock, output, strlen(output));

	pclose(fp);

}

/**
 * Exec cmd and send output to client
 */
void exec_and_echo(char *cmd, int sock) {
//	cmd[strlen(cmd) - 2] = 0; // discard new line, for telnet tests.
	system(cmd);
	write(sock, cmd, strlen(cmd));
}

int main(int argc, char **argv) {
	int listenfd; // socket descriptor to listen to connection requests.
	int connfd; // socket descriptor to connect to client and send data.
	struct sockaddr_in servaddr; // struct that will contain server address info.
	char cmd[MAXDATASIZE]; // buffer that will contain data to be send to client.
	int port;

	struct sockaddr_in client_addr; // struct that will contain remote address info.
	socklen_t client_len = sizeof(client_addr); // addr size

	pid_t pid;

	char client_address[20] = { 0 };
	int client_port;

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
		connfd = Accept(listenfd, (struct sockaddr *) &client_addr,
				&client_len);

		/** create child to answer **/
		if ((pid = fork()) == 0) {
			close(listenfd);

			strcpy(client_address, inet_ntoa(client_addr.sin_addr));
			client_port = ntohs(client_addr.sin_port);
			while (1) {
				bzero(cmd, sizeof(cmd));
				read(connfd, cmd, sizeof(cmd));

				if (!strcmp(cmd, "exit"))
					break;

				printf("%s:%d>> %s\n", client_address, client_port, cmd);
				exec_cmd(cmd, connfd);
			}

			close(connfd);
			printf("%s:%d: Connection closed.\n", client_address, client_port);
			exit(0);
		}

		/** close socket connfd**/
		close(connfd);
	}
	return (0);
}
