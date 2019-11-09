/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "structs.h"


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

	 packet incoming_packet, outgoing_packet;


     if (argc < 3) {
         //fprintf(stderr,"ERROR, no port provided\n");
		 fprintf(stderr,"usage %s port this_machine_name\n", argv[0]);
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;

     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
     bzero(buffer,256);

	 //n = read(newsockfd,buffer,255);
		n = read(newsockfd,&incoming_packet, sizeof (packet));

     if (n < 0) error("ERROR reading from socket");
	 //printf("Here is the message: %s\n",buffer);
	 printf("Message: %sFrom machine: %s\n\n ",incoming_packet.message, incoming_packet.nickname);

	 //bzero (outgoing_packet.message, 256);
	 bzero ((char*) &outgoing_packet, sizeof (packet));
	 strcpy (outgoing_packet.nickname, argv[2]);
	// printf("outgoing_packet.nickname: %s\n", outgoing_packet.nickname);
	 strcpy (outgoing_packet.message, "I got your message");

	 //printf("outgoing_packet.message: %s\n", outgoing_packet.message);
	 //printf("outgoing_packet.nickname: %s\n", outgoing_packet.nickname);
	 n = write(newsockfd,&outgoing_packet,sizeof (packet));

     //n = write(newsockfd,"I got your message",18);
	 if (n < 0) error("ERROR writing to socket");
     close(newsockfd);
     close(sockfd);
     return 0;
}
