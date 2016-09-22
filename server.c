
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


#define max_msg_size 1000



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

//forward messages to clients
    void forward(int sockfd, char *buf, int size_buf)
    {
      struct msg_sbcp msg;
      struct attr_sbcp attr;
      int n;

      if (buf[size_buf]=='\n')
        buf[size_buf]=='\0';

      attr.type = ATTR_MESSAGE;
      attr.length = size_buf + 4;
      for (n=0; n<size_buf; n++)
        attr.payload[n]=buf[n];


      msg.version = 3;
      msg.type = SBCP_SEND;
      msg.payload = &attr;
      msg.length = attr.length + 4;

      if(write(sockfd,&msg,msg.length) < 0)
      {
        perror("Send message error: cannot write socket!\n");
        exit(0);
      }
      printf("Sucessfully send out a message!\n");
    }


    int main(int argc, char *argv[]) 
    {
     int max_clients;
     int i=0;
 //  int port;
     int num_clients = 0;
     int server_sockfd, client_sockfd;
     struct sockaddr_in server_addr;
     int fd;
     char fd_array[max_clients];
     fd_set readfds;

   char input_buffer[max_msg_size + 1];     //socket input buffer
   char kb_buffer[100]; //keyboard input buffer
   
   int sockfd;
   int result;
 //  char hostname[msg_size];
   struct hostent *hostinfo;
   struct sockaddr_in addr;
//   char alias[msg_size];
   int clientid;

   int max_fd;

   int server_port;
   


   if(argc != 4)
   {
    printf("Invalid parameter.\nUsage: %s server_ip server_port max_clients\n", argv[0]);
    exit(0);
  }


  server_port = atoi(argv[2]);
  max_clients = atoi(argv[3]);




  printf("\n*** Server program starting (enter \"quit\" to stop): \n");
  fflush(stdout);

     /* Create and name a socket for the server */

  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_sockfd < 0)
  {
    printf("Error creating socket\n");
  }

  memset(&server_addr, '0', sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(server_port);
  memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);  

  
  if((bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
  {
    printf("Error binding\n");
    exit(0);
  }
  else{
    printf("Server binding suceed!\n");
  }

     /* Create a connection queue and initialize a file descriptor set */

    //Listen for incoming connections
  listen(server_sockfd, 5);

  if(listen(server_sockfd, 5) < 0)
  {
    printf("Error listening\n");
  }
  
  FD_ZERO(&readfds);
  FD_SET(server_sockfd, &readfds);
  FD_SET(0, &readfds);  /* Add keyboard to file descriptor set */


     /*  Now wait for clients and requests */
  while (1) 
  {

    max_fd = server_sockfd;

    for(i=0;i<num_clients;i++)
    {
        /*dont write msg to same client*/
      if (fd_array[i] > max_fd)
        max_fd =  fd_array[i];

    }
        /* select API */
    select(max_fd + 1, &readfds, NULL, NULL, NULL);


    /* Accept a new connection request */
    if (FD_ISSET(server_sockfd, &readfds)) 
    { 
     printf("receive from client");

     client_sockfd = accept(server_sockfd, NULL, NULL);

     printf("client_sockfd: %d\n",client_sockfd);


      if (msg->type == SBCP_JOIN){
      fd_array[num_clients++]=client_sockfd;

        //TODO: print out the name of user or inform other users
      struct attr_sbcp* pattr = msg->payload;

        /*Client ID*/
      printf("Client %d joined\n",num_clients);
      char join_msg[100];

      fflush(stdout);
    } else if (msg->type == SBCP_SEND){//forward message to other clients
       char* output = input_buffer + 8;

       for(i=0;i<num_clients;i++)
       {
         /*dont write msg to same client*/
        if (fd_array[i] != fd)  
          forward(fd_array[i],output,strlen(output));
      }

    }

     if (num_clients < max_clients) 
     {
      FD_SET(client_sockfd, &readfds);

      result = read(client_sockfd, input_buffer, max_msg_size);

      if(result==-1){
       perror("Cannot read from client socket!");
       error(0);
     }else //too many clients
     {
      char error_msg[100];

      sprintf(error_msg, "Sorry, too many clients.  Try again later.\n");
      error_msg[44] = '\0';

      forward(client_sockfd, error_msg, strlen(error_msg));
      close(client_sockfd);
    }

    struct msg_sbcp *msg = (struct msg_sbcp*) input_buffer;

   

  }
  }else if (fd == 0)  
  {  /* Process keyboard activity */                 
  fgets(kb_buffer, sizeof(kb_buffer), stdin);
  printf("%s\n",kb_buffer);

  if (strcmp(kb_buffer, "quit")==0) {
    char error_msg[100];

    sprintf(error_msg, "Server is quiting.\n");
    error_msg[19] = '\0';

    for (i = 0; i < num_clients ; i++)
     forward(fd_array[i], error_msg, strlen(error_msg));
  }
}

              /*Process Client specific activity*/
  /*            else if(fd) 
              {  
  
                 
                 result = read(fd, msg, msg_size);
                 
                 if(result==-1)
                   perror("read()");
                 else if(result>0)
                 {
                    
                    sprintf(kb_msg,"M%2d",fd);
                    msg[result]='\0';
                    
                    
                    strcat(kb_msg,msg+1);                                        
                    
        
                    for(i=0;i<num_clients;i++)
                    {
            
                       if (fd_array[i] != fd)  
                          forward(fd_array[i],kb_msg,strlen(kb_msg));
                    }
      
                    printf("%s",kb_msg+1);
                    
               
                    if(msg[0] == 'X')
                    {
                       exitClient(fd,&readfds, fd_array,&num_clients);
                    }   
                 }                                   
              }  */                
             /* else 	
              {  
          
                 exitClient(fd,&readfds, fd_array,&num_clients);
              }*/
               }
             }


