#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "structs.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	packet incoming_packet, outgoing_packet;

    char buffer[256];
    if (argc < 4) {
       fprintf(stderr,"usage %s server_ip_address port this_machine_name\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

	//printf ("server ip is %ul\n",server->h_addr );

    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);

	//memcpy (outgoing_packet.message, buffer, 256);
	bzero ((char*) &outgoing_packet, sizeof (packet));
	strcpy (outgoing_packet.nickname, argv[3]);
	strcpy (outgoing_packet.message, buffer);
	n = write(sockfd,&outgoing_packet,sizeof(packet));


    //n = write(sockfd,buffer,strlen(buffer));

    if (n < 0)
         error("ERROR writing to socket");
    bzero(buffer,256);
    //n = read(sockfd,buffer,255);
	n = read(sockfd,&incoming_packet,sizeof(packet));

    if (n < 0)
         error("ERROR reading from socket");
    //printf("%s\n",buffer);
	//printf("%s\n",incoming_packet.message);
	 //printf("incoming_packet.nickname: %s\n", incoming_packet.nickname);
	// printf("incoming_packet.message: %s\n", incoming_packet.message);

	printf("Message: %s\nFrom machine: %s\n\n ",incoming_packet.message, incoming_packet.nickname);

    close(sockfd);
    return 0;
}
