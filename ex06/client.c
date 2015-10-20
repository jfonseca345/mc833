#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 4096

int main(int argc, char **argv) {
   int sockfd; // socket descriptor.
   int n; // size of string received from server.
   char   recvline[MAXLINE + 1]; // array of chars that will store data received from server.
   char   error[MAXLINE + 1]; // string that will be used to show error messages to user.
   struct sockaddr_in servaddr; // struct to store server internet address.

   struct sockaddr_in getsock; // struct that will be used to get local socket info.
   socklen_t len = sizeof(struct sockaddr); // sockeadd_in struct len.

   /** Show error message if usage in incorrect. **/
   if (argc != 2) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      perror(error);
      exit(1);
   }

   /** Creates a IPV4 TCP socket for client and its return for error checking.**/
   if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket error");
      exit(1);
   }

   bzero(&servaddr, sizeof(servaddr)); // inialize struct to all zeros.
   servaddr.sin_family = AF_INET; // set address family to IPV4.
   servaddr.sin_port   = htons(9999); // set port to 9999, htons convert from host to server representation of short int.

   /** convert address informed in comand line argv[1] to a binary representation to be stored in 
    *  sockaddr_in struct in sin_addr field. Also verify return for error checking. **/
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(1);
       }
   /** starts connection with server address stored in servaddr using socket created 
    *  before and described by sockfd. Alsi verify return for error checking. **/
   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
      perror("connect error");
      exit(1);
   }

   /** Read data from socket to recvline. Size is returned to n.**/
   while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
      recvline[n] = 0;
      /** Print data read to std out.**/
      if (fputs(recvline, stdout) == EOF) {
         perror("fputs error");
         exit(1);
      }
   }

   /** print error if no data was read. **/
   if (n < 0) {
      perror("read error");
      exit(1);
   }

   /** get socket info related to sockfd, and put information in getsock. */
   getsockname(sockfd, (struct sockaddr *) &getsock, &len);

   /** print to stout, making data representation convertion from network to host. **/
   printf("%s : %d\n", inet_ntoa(getsock.sin_addr), ntohs(getsock.sin_port));

   exit(0);
}
