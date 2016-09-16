//The client side program for SBCP based chat program
a
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/type.h>
#include <sys/sock.h>
#include <netinet.h>
#include <netdb.h>

#include "sbcp.h"


int main(int argc, char *argv[])
{
    int sockfd, server_port;
    char* username;
    struct hostent *server_ip;
    struct sockaddr_in server_addr;

    //sbcp structure
    struct attr_sbcp attr;
    struct msg_sbcp msg;

    fd_set read_fds;        //Read File Description set

    //buffer for sending message payload
    char join_buffer[20];
    char message_buffer[512];
    char sock_buffer[1000];

    //buffer for receiving message
    char rev_buffer[1000];

    if (argc < 4){
        fprintf(stderr, "usage %s username server_ip server_port", argv[0]);
        exit(0);
    }
    
    username = argv[1];
    server_ip = gethostbyname(argv[2]);
    server_port = atoi(argv[3]);
    if (server_ip == NULL){
        fprintf(stderr, "No such server.");
        exit(0);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
        
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sockfd, &server_addr, sizeof(server_addr)){
        close(&sockfd);
        error("Cannot connect to server! \n Exit ...");           exit (0);
    }

    //TODO: Client send JOIN msg to server
    strcpy(join_buffer, username);
    attr.type = ATTR_USERNAME;
    attr.payload = join_buffer;
    attr.length = sizeof(attr);

    msg.version = 3;
    msg.type = SBCP_JOIN;
    msg.payload = &attr;
    msg.length = sizeof(msg);

    memcpy(sock_buffer, msg, msg.length);

    if (send(sockfd, sock_buffer, sizeof(sock_buffer)) == -1){
        fprintf(stderr, "Cannot send out JOIN message. Quit...");
        exit(0);

    }

    FD_SET(sockfd, &read_fds);   //add socket fd
    FD_SET(0, &read_fds);        //add stdin fd


    while (true){
        if (select(sockfd + 1， &read_fds, 0, 0, 0) < 0){
            fprintf(stderr, "Cannot select file descriotions. ");
            exit(0);
        }

        //TODO: process user keyborad input
        if (FD_ISSET(0, &read_fds)){
            
        }

        //process network socket input
        if (FD_ISSET(fd_sock, &read_fds)){
            if ((num = recv(fd_sock, recv_buffer, 1000, 0)) <=0){
                fpfrintf(stderr, "Error in receving message from server.");
            }
            recv_buffer[num] = "\0";
            printf("%s\n", recv_buffer);
        }
    }


    return 0;
    }
}
