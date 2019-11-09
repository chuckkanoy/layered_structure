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
    packet wire_packet;
    frame wire_frame;
    int n;

    while (1)
    {
        /*add codes receive a frame from wire*/
        n = read(wiresockfd,&wire_frame,sizeof(frame));
        if (n < 0)
        {
            printf("%d\n", wiresockfd);
            error("ERROR reading from socket");
        }

        /*add codes to send the included packet to the network layer*/
        bzero ((char*) &wire_packet, sizeof (packet));
        strcpy (wire_packet.nickname, wire_frame.my_packet.nickname);
        strcpy (wire_packet.message, wire_frame.my_packet.message);
        n = write(network_layersockfd,&wire_packet,sizeof(packet));
        if (n < 0)
            error("ERROR writing to socket");
    }
}


int main(int argc, char *argv[])
{
    /*add codes to declear local variables*/
    int cli_addr, clilen, portno, n, newsockfd;
    int threadlist[2];
    struct hostent *server;
    struct sockaddr_in serv_addr;
    char buffer[256];
    packet incoming_packet, network_packet, wire_packet;
    frame network_frame, wire_frame;

    /*//look at arguments given
    int size, i;
    size = argc;
    printf("%d\n", argc);
    for(i = 0; i < size; i++)
    {
        printf("%s\n", argv[i]);
    }*/

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

	//printf ("server ip is %ul\n",server->h_addr );

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
     portno = atoi(argv[1]);
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
        n = read(newsockfd,&network_packet,sizeof(packet));
        printf("read network\n");
        if (n < 0)
        {
            error("ERROR reading from socket");
        }

        /* add codes to wrap the packet into a frame*/
        network_frame.seq_num = 0;
        network_frame.type = 0;
        network_frame.my_packet = network_packet;

        /*add codes to send the frame to the wire*/
        n = write(wiresockfd, &network_frame, sizeof(frame));

        /*if the message is "EXIT" */
        if (strcmp (incoming_packet.message, "EXIT\n")==0)
        {
            pthread_cancel(wirepth); //kill the child thread
            close(wiresockfd);
            close (network_layersockfd); //close sockets
            return 0; //terminate the main function
        }

    }

}
