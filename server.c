
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
        buf[size_buf] = '\0';

      attr.type = ATTR_MESSAGE;
      attr.length = size_buf + 4;
      for (n=0; n<size_buf; n++)
        attr.payload[n]=buf[n];


      msg.version = 3;
      msg.type = SBCP_SEND;
      msg.payload = attr;
      msg.length = attr.length + 4;

      if(send(sockfd,&msg,msg.length,0) < 0)
      {
        perror("Send message error: cannot write socket!\n");
        close(sockfd);
        exit(0);
      }
      printf("Sucessfully send out a message to %d!\n", sockfd);
    }


    int main(int argc, char *argv[]) 
    {
     int max_clients;
     int i=0, j=0;
 //  int port;
     int num_clients = 0;
     int server_sockfd, client_sockfd;
     struct sockaddr_in server_addr;
     int fd;
  
     fd_set readfds,activefds;

   char input_buffer[max_msg_size + 1];     //socket input buffer
   char kb_buffer[100]; //keyboard input buffer
   
   int sockfd;
   int result;

   struct hostent *hostinfo;
   struct sockaddr_in addr;
    struct sockaddr_in client_addr;
    size_t size;
   int clientid;

   int fd_max;

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

    //Listen for #include "server.h"oming connections
  listen(server_sockfd, 5);

  if(listen(server_sockfd, 5) < 0)
  {
    printf("Error listening\n");
  }

  FD_ZERO(&readfds);
  FD_ZERO(&activefds);
  FD_SET(server_sockfd, &activefds);
//  FD_SET(0, &activefds);  /* Add keyboard to file descriptor set */

  fd_max = server_sockfd;

  // name for users
  char client_names[max_clients][100];
    char fd_array[max_clients];

     /*  Now wait for clients and requests */
  while (1) 
  {
  readfds = activefds;
        /* select API */
  select( fd_max +1, &readfds, NULL, NULL, NULL);

  for(i=0;i< fd_max + 1;i++)
  {
    if (FD_ISSET(i, &readfds)){
   /* if (0 == i)  
    {          
      fgets(kb_buffer, sizeof(kb_buffer), stdin);
    printf("%s\n",kb_buffer);

    if (strcmp(kb_buffer, "quit")==0) {
      char error_msg[100];

      sprintf(error_msg, "Server is quiting.\n");
      error_msg[19] = '\0';

      for (j = 0; j < num_clients ; j++)
      forward(fd_array[j], error_msg, strlen(error_msg));
      }
    }
   else*/ if (i == server_sockfd)    /* Accept a new connection request */
    { 
     printf("receive from client\n");

     size = sizeof(client_addr);
     client_sockfd = accept(server_sockfd,  (struct sockaddr *)&client_addr,(socklen_t*)&size);

       printf("client_sockfd: %d\n", client_sockfd);
       fflush(stdout);

       FD_SET(client_sockfd, &activefds);

       if (client_sockfd > fd_max){
         fd_max = client_sockfd;
       }
  }
  else {
        if (recv(i,input_buffer,sizeof(input_buffer),0) <= 0){
            break;
        }

        struct msg_sbcp *msg = (struct msg_sbcp*) input_buffer;

        if (msg->type == SBCP_JOIN){

          if (num_clients < max_clients) 
         {  
             char username[100];
             int occupied = 0;

            struct attr_sbcp attribute =  msg->payload;

            printf("the len: %d\n", attribute.length);
            username[attribute.length - 4] = '\0';

          
            for (j=0; j < attribute.length - 4; j++){
              username[j] = attribute.payload[j];
              }

            //if the username is occupied
            for (j=0; j<num_clients; j++){
              if (strcmp(username,  client_names[j]) == 0){
                  occupied = 1;
              }
            }
           
            if (occupied == 0){//if the username is not occupied
              fd_array[num_clients] = i;
              strcpy(client_names[num_clients++], username);

              //debug username
              printf("The username is %s.\n",client_names[num_clients-1]);
            }
            else{
              printf("The name is used!");

              char error_msg[] = "The name is used! Close socket!";
              forward(i,error_msg, strlen(error_msg));

               FD_CLR(i, &activefds);
               close(i);

            }
            fflush(stdout); 
        }
         else{
            printf("Too many clients!");

              char error_msg[] = "Too many clients! Close socket!";
              forward(i,error_msg, strlen(error_msg));

               FD_CLR(i, &activefds);
               close(i);
         }
          }
    

         if (msg->type == SBCP_SEND){//forward message to other clients
            
             struct attr_sbcp attribute =  msg->payload;
  //           printf("the len: %d\n", attribute.length);
             fflush(stdout); 

             char send_buf[100];
              int m;
              //attach username to msg
              for (m =0; m < num_clients; m++){
              if (fd_array[m] == i){
          //      printf("Found user name ");
          //    fflush(stdout);
                strcpy(send_buf, client_names[m]);
              }
            }
            
            int len = strlen(send_buf);
       
            send_buf[len] = ':';
            send_buf[len+1] = ' ';

            for (m=0; m< attribute.length - 4; m++){
                send_buf[m+len+2] = attribute.payload[m];
            }
            
      //     printf("receive message from client");
      //     fflush(stdout); 

           printf("%s %d\n",send_buf, len + attribute.length - 2);
           fflush(stdout);
           for(j=1;j<fd_max+1;j++)
          {
            /*dont write msg to same client*/
            if (FD_ISSET(j, &activefds) && j != server_sockfd && j != i) {
               forward(j,send_buf, len + attribute.length - 2);

               }
            }
          
        }
   }
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


