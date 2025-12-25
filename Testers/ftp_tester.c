#include "ftp_tester.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int run_ftp_test(const char* address, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr; 
    char buffer[1024] = {0};

    serv_addr.sin_family = AF_INET; //IPv4 addresses
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, address, &serv_addr.sin_addr);
    
    //establish TCP connection
    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        return -1;
    }

    int valread = recv(sock, buffer, 1024, 0);
    close(sock);


    if(valread > 0 && strstr(buffer, "220") != NULL){
        return 1; // Succes
    }
    return 0;
}