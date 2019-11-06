#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "structs.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/*Global variables that will be accessed in the thread function  */
int clientlist[2]; /*the socket numbers through with the 2 clients (e.g. data link layer) are connected to this wire*/


/*the thread function that will receive frames from socket (i.e. data_link layer) and send the received frames to another socket*/
void * onesocket ( int threadsockfd)
{
    /*add codes to declear local variables*/
    char buffer[256];
    int n;
    packet incoming_packet, outgoing_packet;


    while (1)
    {
        /*add codes to receive a frame from threadsocketfd*/
        bzero(buffer,256);
        n = read(threadsockfd,&incoming_packet,sizeof(packet));
        if (n < 0)
            error("ERROR reading from socket");

        //if frame message is EXIT then terminate thread and return NULL
        if(strcmp(incoming_packet.message, "EXIT") == 0)
        {
            close(threadsockfd);
            return NULL;
        }
        //otherwise write message to other socket in clientlist
        else
        {
            if(clientlist[0] == threadsockfd)
            {
                //copy message over from incoming to outgoing
                bzero ((char*) &outgoing_packet, sizeof (packet));
                strcpy (outgoing_packet.nickname, incoming_packet.nickname);
                strcpy (outgoing_packet.message, incoming_packet.message);
                //write outgoing packet to socket
                n = write(clientlist[1], &outgoing_packet, sizeof(packet));
                //display error if necessary
                if(n < 0)
                    error("ERROR writing to socket");
            } else if (clientlist[1] == threadsockfd){
                //copy message over from incoming to outgoing
                bzero ((char*) &outgoing_packet, sizeof (packet));
                strcpy (outgoing_packet.nickname, incoming_packet.nickname);
                strcpy (outgoing_packet.message, incoming_packet.message);
                //write outgoing packet to socket
                n = write(clientlist[0], &outgoing_packet, sizeof(packet));
                //display error if necessary
                if(n < 0)
                    error("ERROR writing to socket");
            }
        }

    }
}

int main(int argc, char *argv[])
{
    /*add codes to declare local variables*/
    int newsockfd, sockfd, cli_addr, clilen, i, portno;
    int threadlist[2];
    struct sockaddr_in serv_addr;

    /*check the number of arguments*/
    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    /*add codes to create a socket (sockfd), bind its address to it and listen to it*/
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

    for (i=0; i<2; i=i+1) /*only accept two requests*/
    {
        /*accept a request from the data link layer*/
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        /* store the new socket into clientlist*/
        clientlist[i]=newsockfd;

        /*creat a thread to take care of the new connection*/
        pthread_t pth;	/* this is the thread identifier*/
        pthread_create(&pth,NULL,onesocket,clientlist[i]);
        threadlist[i]=pth; /*save the thread identifier into an array*/
    }
    close(sockfd); /*so that wire will not accept further connection request*/
    pthread_join(threadlist[0],NULL);
    pthread_join(threadlist[1],NULL); /* the main function will not terminated untill both threads finished*/
    return 0;

}
