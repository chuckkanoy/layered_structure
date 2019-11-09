#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "structs.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

/*the thread function to receive packets from the data link layer and display the messages*/
void * rcvmsg (int threadsockfd)
{
    /*add codes for local varialbes*/
    int n;
    packet incoming_packet, outgoing_packet;

    while (1)
    {
        /*add codes to read a packet from threadsockfd and display it to the screen*/
        n = read(threadsockfd,&incoming_packet,sizeof(packet));
        if (n < 0)
            error("ERROR reading from socket");
        printf("Message: %sFrom machine: %s\n\n ",incoming_packet.message, incoming_packet.nickname);

    }
    return NULL;
}


int main(int argc, char *argv[])
{
    /*add codes for local variables*/
    int newsockfd, sockfd, cli_addr, clilen, n, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    packet incoming_packet, outgoing_packet;

    /*
    //look at arguments given
    int size, i;
    size = argc;
    printf("%d\n", argc);
    for(i = 0; i < size; i++){
        printf("%s\n", argv[i]);
    }*/

    /*check number of aruguments*/
    if (argc < 4)
    {
        fprintf(stderr,"usage %s data_add data_port nickname\n", argv[0]);
        exit(0);
    }

    /*add codes to connect to the data link layer*/
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    printf("%d\n", sockfd);
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

    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");


    /*creat a thread to receive packets from the data link layer*/
    pthread_t pth;	// this is our thread identifier
    pthread_create(&pth,NULL,rcvmsg,sockfd);

    /* the main function will receive messages from keyboard and send packets to the data link layer*/
    while (1)
    {
        /*add codes to receive a message from keyboard, wrap it into a packet and send it to the data link layer*/
        printf("Ready to communicate: \n");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        bzero ((char*) &outgoing_packet, sizeof (packet));
        strcpy (outgoing_packet.nickname, argv[3]);
        strcpy (outgoing_packet.message, buffer);
        n = write(sockfd,&outgoing_packet,sizeof(packet));
        if (n < 0)
            error("ERROR writing to socket");
        /*if the meassge is "EXIT"*/
        if (strcmp (buffer, "EXIT\n")==0)
        {
            pthread_cancel(pth); //kill the child thread
            close(sockfd); // close socket
            return 0;	//terminate
        }
    }

}
