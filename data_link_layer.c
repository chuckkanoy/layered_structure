#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "structs.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/*global variables to be used in the threads*/
int network_layersockfd;//the socket through which the network layer is connected.
int wiresockfd; //the socket through which the wire is connected.


/*the thread function that receives frames from the wiresocket and sends packets to the network_layer */
void * rcvfromwiresend2network_layer ( char *argv[] )
{
    /*add codes to declare locals*/
    packet outgoing_packet;
    frame incoming_frame;
    int n;

    while (1)
    {
        /*add codes receive a frame from wire*/
        n = read(wiresockfd,&incoming_frame,sizeof(frame));
        if (n < 0)
        {
            error("ERROR reading from socket");
        }

        //display frame info on screen
        printf("Received a frame from wire.\n\tSequence number: %d\n\tFrame type: %d\n\tSending the included packet to network_layer...\n\n", incoming_frame.seq_num, incoming_frame.type);

        /*add codes to send the included packet to the network layer*/
        bzero ((char*) &outgoing_packet, sizeof (packet));
        strcpy (outgoing_packet.nickname, incoming_frame.my_packet.nickname);
        strcpy (outgoing_packet.message, incoming_frame.my_packet.message);
        network_layersockfd = 5;
        n = write(network_layersockfd,&outgoing_packet,sizeof(packet));
        if (n < 0)
            error("ERROR writing to socket");
    }
}


int main(int argc, char *argv[])
{
    /*add codes to declear local variables*/
    int clilen, portno, n, newsockfd, seq_num = 0;
    int threadlist[2];
    struct hostent *server;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[256];
    packet incoming_packet;
    frame outgoing_frame;

    /*check numeber of arguments*/
    if (argc < 4)
    {
        fprintf(stderr,"Usage: %s  wire__IP  wire_port data_port\n",argv[0] );
        exit(1);
    }

    /* add codes to connect to the wire. Assign value to gobal varialbe wiresockfd */
    portno = atoi(argv[2]);
    wiresockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (wiresockfd < 0)
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
    serv_addr.sin_port = htons(portno);
    if (connect(wiresockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    /*generate a new thread to receive frames from the wire and pass packets to the network layer */
    pthread_t wirepth;	// this is our thread identifier
    pthread_create(&wirepth,NULL,rcvfromwiresend2network_layer, NULL);

    /*add codes to create and listen to a socket that the network_layer will connect to. Assign value to global variable network_layersockfd*/
    network_layersockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (network_layersockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[3]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;

     serv_addr.sin_port = htons(portno);
     if (bind(network_layersockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(network_layersockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(network_layersockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
    printf("create new socket: %d\n", newsockfd);
    bzero(buffer,256);
    /*the main function will receive packets from the network layer and pass frames to wire*/
    while (1)
    {
        /*add codes to receive a packet from the network layer*/
        bzero(buffer,256);
        n = read(newsockfd,&incoming_packet,sizeof(packet));
        if (n < 0)
        {
            error("ERROR reading from socket");
        }

        //print status
        printf("Received a packet from network_layer\nSending a frame to wire...\n\n");

        /* add codes to wrap the packet into a frame*/
        outgoing_frame.seq_num = seq_num;
        outgoing_frame.type = 0;
        outgoing_frame.my_packet = incoming_packet;

        /*add codes to send the frame to the wire*/
        n = write(wiresockfd, &outgoing_frame, sizeof(frame));

        //iterate sequence number
        seq_num++;

        /*if the message is "EXIT" */
        if (strcmp (incoming_packet.message, "EXIT\n")==0)
        {
            printf("entered EXIT\n");
            pthread_cancel(wirepth); //kill the child thread
            close(wiresockfd);
            close (network_layersockfd); //close sockets
            return 0; //terminate the main function
        }

    }

}
