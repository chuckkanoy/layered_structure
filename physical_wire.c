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
	//...


	 while (1)
	 {
		/*add codes to receive a frame from threadsocketfd*/
		//...


		/*if the message in the frame in EXIT close the socket and terminate this thread using
				close(threadsockfd);
				return NULL;
		 other wise send the frame to the other socket (which is stored in clientlist)
		*/
		//...

	 }
}

int main(int argc, char *argv[])
{
	/*add codes to declare local variables*/
	int newsockfd, sockfd, cli_addr, clilen, i, portno;
	int threadlist[2];

	/*check the number of arguments*/
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

	/*add codes to create a socket (sockfd), bind its address to it and listen to it*/
	portno = atoi(argv[2]); //converts argv[2] to an integer
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR Opening Socket");


	for (i=0;i<2;i=i+1) /*only accept two requests*/
	{
		/*accept a request from the data link layer*/
		 newsockfd = accept(sockfd,
					 (struct sockaddr *) &cli_addr,
					 &clilen);

		// ...
		/* store the new socket into clientlist*/
		 clientlist[i]=newsockfd;

		//...
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
