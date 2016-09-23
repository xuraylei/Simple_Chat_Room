//The client side program for SBCP based chat program
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "sbcp.h"


int main(int argc, char *argv[])
{
    int sockfd, server_port,i,j;
    char* username;
    struct hostent *server_ip;
    struct sockaddr_in server_addr;

    //sbcp structure
    struct attr_sbcp attr;
    struct msg_sbcp msg;

    fd_set read_fds;        //Read File Description set

    char input_buffer[100]; //the buffer for user inpout

    //buffer for sending message payload
    char join_buffer[20];
    char message_buffer[512];  // the buffer for SEND message
    char sock_buffer[1000];

    //buffer for receiving message
    char recv_buffer[1000];

    if (argc < 4){
        fprintf(stderr, "usage: %s username server_ip server_port\n", argv[0]);
        exit(0);
    }
    
    memset(&server_addr, '0', sizeof(server_addr)); 

    username = argv[1];
    server_ip = gethostbyname(argv[2]);
    server_port = atoi(argv[3]);
    if (server_ip == NULL){
        fprintf(stderr, "No such server.");
        exit(0);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[2]);
    server_addr.sin_port = htons(server_port);
    memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);  
        
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("Cannot connect to server! Exit ...");         
        close(sockfd);
        exit (0);
    }
    else{
          fprintf(stdin, "connected to server %s successfully!", argv[1]);
    }
 
    //Client send JOIN msg to server

    attr.type = ATTR_USERNAME;
    attr.length = strlen(username) + 4;
    for (i=0;i<strlen(username);i++)
         attr.payload[i]=username[i];

    msg.version = 3;
    msg.type = SBCP_JOIN;
    msg.payload = attr;
    msg.length = attr.length + 4;

    //memcpy(sock_buffer, msg, msg.length);
    if (write(sockfd, (char*)&msg, msg.length) < 0){
        perror("Cannot send out JOIN message. Quit...");
        exit(0);
    }
    else{
        //debug
 //       printf("Send out Join Successfully!\n");
  //      fflush(stdout);
    }

    FD_CLR(sockfd, &read_fds);
    FD_SET(sockfd, &read_fds);   //add socket fd
    FD_SET(0, &read_fds);        //add stdin fd

    while (1){
        if (select(sockfd+1, &read_fds, 0, 0, 0) < 0) {
            perror("Cannot select file descriotions. ");
            exit(0);
        }
        //process user keyborad input and send to server side
        if (FD_ISSET(0, &read_fds)){
            
            //Client send SEND message to server
            fgets(input_buffer, sizeof(input_buffer), stdin);

            int size_buf=strlen(input_buffer)-1;
            if (input_buffer[size_buf]=='\n')
              input_buffer[size_buf]='\0';

            attr.type = ATTR_MESSAGE;
            attr.length = size_buf + 4;

            for (i=0;i<size_buf;i++)
                attr.payload[i]=input_buffer[i];

            msg.version = 3;
            msg.type = SBCP_SEND;
            msg.payload = attr;
            msg.length = attr.length + 4;

        //debug
         //   printf("send out message with len %d", msg.length);
         //   fflush(stdout);

            if (write(sockfd, &msg, msg.length) < 0){
                perror("Cannot send out user message. Quit...");
                exit(0);
            }
             else{
                //debug
           //      printf("Send out Message Successfully!");
            //     fflush(stdout);
        }
        }
        
        //process network socket input
        int num = -1;
        if (FD_ISSET(sockfd, &read_fds)){
            if ((num = recv(sockfd, recv_buffer, 1000, 0)) <= 0){
                perror("Error in receving message from server.");
                exit(0);
            }
            struct msg_sbcp *msg = (struct msg_sbcp*) recv_buffer;
            struct attr_sbcp attribute =  msg->payload;

            printf("receive length %d", attribute.length);
            fflush(stdout);

            for (j=0; j < attribute.length -4; j++){
             printf("%c", attribute.payload[j]);
            }
            printf("\n");

            fflush(stdout); 
        }
    }

    close(sockfd);
    return 0;
    
}
