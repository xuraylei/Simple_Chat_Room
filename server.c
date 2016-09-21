


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>		// defines perror(), herror() 
#include <fcntl.h>		// set socket to non-blocking with fcntrl()
#include <unistd.h>
#include <string.h>
#include <assert.h>		//add diagnostics to a program

#include <netinet/in.h>		//defines in_addr and sockaddr_in structures
#include <arpa/inet.h>		//external definitions for inet functions
#include <netdb.h>		//getaddrinfo() & gethostbyname()

#include <sys/socket.h>		//
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/select.h>		// for select() system call only
#include <sys/time.h>		// time() & clock_gettime()

#include "sbcp.h"


#define msg_size 80
#define max_clients 10
#define myport 7400


void exitClient(int fd, fd_set *readfds, char fd_array[], int *num_clients)
{
    int i;
    
    close(fd);
    FD_CLR(fd, readfds); //clear the leaving client from the set
    for (i = 0; i < (*num_clients) - 1; i++)
        if (fd_array[i] == fd)
            break;          
    for (; i < (*num_clients) - 1; i++)
        (fd_array[i]) = (fd_array[i + 1]);
    (*num_clients)--;
}

int main(int argc, char *argv[]) 
{
   int i=0;
   int port;
   int num_clients = 0;
   int server_sockfd, client_sockfd;
   struct sockaddr_in server_addr;
   int addrlen = sizeof(struct sockaddr_in);
   int fd;
   char fd_array[max_clients];
   fd_set readfds, testfds, clientfds;
   char msg[msg_size + 1];     
   char kb_msg[msg_size + 10]; 
   
   int sockfd;
   int result;
   char hostname[msg_size];
   struct hostent *hostinfo;
   struct sockaddr_in addr;
   char alias[msg_size];
   int clientid;
   
 
  
   if(argc==1 || argc == 3)
   {
     if(argc==3)
     {
       if(!strcmp("-p",argv[1]))
       {
         sscanf(argv[2],"%i",&port);
       }
       else
       {
         printf("Invalid parameter.\nUsage: chat [-p PORT] HOSTNAME\n");
         exit(0);
       }
     }
     else port= myport;
     
     printf("\n*** Server program starting (enter \"quit\" to stop): \n");
     fflush(stdout);

     /* Create and name a socket for the server */
     
     server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
     if (server_sockfd < 0)
     {
     	printf("Error creating socket\n");
     }
     
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     server_addr.sin_port = htons(port);
   
    /* binding */
     bind(server_sockfd, (struct sockaddr *)&server_addr, addrlen);
     
     if((bind(server_sockfd, (struct sockaddr *)&server_addr, addrlen)) < 0)
     {
     	printf("Error binding\n");
     }

     /* Create a connection queue and initialize a file descriptor set */
     
     /* Listen for incoming connections
     listen(server_sockfd, 5);
     
     if(listen(server_sockfd, 5) < 0
     {
     	printf("Error listening\n");
     }
  
     FD_ZERO(&readfds);
     FD_SET(server_sockfd, &readfds);
     FD_SET(0, &readfds);  /* Add keyboard to file descriptor set */
 
     
     
     /*  Now wait for clients and requests */
     while (1) 
     {
        testfds = readfds;
        
        /* select API */
        select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
                    
        /* If there is activity, find which descriptor it's on using FD_ISSET */
        for (fd = 0; fd < FD_SETSIZE; fd++) 
        {
           if (FD_ISSET(fd, &testfds)) 
           {
              /* Accept a new connection request */
              if (fd == server_sockfd) 
              { 
                 client_sockfd = accept(server_sockfd, NULL, NULL);
                 
                 printf("client_sockfd: %d\n",client_sockfd);
                
                                
                 if (num_clients < max_clients) 
                 {
                    FD_SET(client_sockfd, &readfds);
                    fd_array[num_clients]=client_sockfd;
                   
                    /*Client ID*/
                    printf("Client %d joined\n",num_clients++);
                    fflush(stdout);
                    
                    sprintf(msg,"M%2d",client_sockfd);
                    
                    /*write 2 byte clientID */
                    send(client_sockfd,msg,strlen(msg),0);
                 }
                 else 
                 {
                    sprintf(msg, "XSorry, too many clients.  Try again later.\n");
                    write(client_sockfd, msg, strlen(msg));
                    close(client_sockfd);
                 }
              }
              else if (fd == 0)  
              {  /* Process keyboard activity */                 
                 fgets(kb_msg, msg_size + 1, stdin);
                 //printf("%s\n",kb_msg);
                 
                 if (strcmp(kb_msg, "quit\n")==0) 
                 {
                    sprintf(msg, "XServer is shutting down.\n");
                    for (i = 0; i < num_clients ; i++) {
                       write(fd_array[i], msg, strlen(msg));
                       close(fd_array[i]);
                    }
                    close(server_sockfd);
                    exit(0);
                 }
                 else 
                 {
                    //printf("server - send\n");
                    sprintf(msg, "M%s", kb_msg);
                    for (i = 0; i < num_clients ; i++)
                       write(fd_array[i], msg, strlen(msg));
                 }
              }
              
              /*Process Client specific activity*/
              else if(fd) 
              {  
  
                 /*read data from open socket*/
                 result = read(fd, msg, msg_size);
                 
                 if(result==-1) perror("read()");
                 else if(result>0)
                 {
                    /*read 2 bytes client id*/
                    sprintf(kb_msg,"M%2d",fd);
                    msg[result]='\0';
                    
                    /*concatinate the client id with the client's message*/
                    strcat(kb_msg,msg+1);                                        
                    
                    /*print to other clients*/
                    for(i=0;i<num_clients;i++)
                    {
                    	/*dont write msg to same client*/
                       if (fd_array[i] != fd)  
                          write(fd_array[i],kb_msg,strlen(kb_msg));
                    }
                    /*print to server*/
                    printf("%s",kb_msg+1);
                    
                     /*Exit Client*/
                    if(msg[0] == 'X')
                    {
                       exitClient(fd,&readfds, fd_array,&num_clients);
                    }   
                 }                                   
              }                  
              else 	
              {  
              /* A client is leaving */
                 exitClient(fd,&readfds, fd_array,&num_clients);
              }
           }
        }
     }
  }

}
