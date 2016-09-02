//The client side program for SBCP based chat program

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/type.h>
#include <sys/sock.h>
#include <netinet.h>
#include <netdb.h>


int main(int argc, char *argv[])
{
    int sockfd, server_port;
    char* username;
    struct hostent *server_ip;
    struct sockaddr_in server_addr;

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
    //TODO: add server ip to the sockaddr
    sockfd = socket(AF_INET, SOCK_STREAM, 0);




    return 0;
}
