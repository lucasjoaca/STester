// http_tester.c
#include "http_tester.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int run_http_test(const char* address, int port) {
    //ipv 4 -> AF_INET
    //TCp -> SOCKSTREAM
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    inet_pton(AF_INET, address, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
    }

    // !  \r\n\r\n tells the web server that this is the request
    // ! and that im waiting for an answer
    char *http_request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    send(sock, http_request, strlen(http_request), 0);

    // read  the answer
    int valread = recv(sock, buffer, 1024, 0);
    close(sock);

    //check if its an ok signal or not (between 200 and 299)
    if (valread > 0 && strstr(buffer, "200 OK") != NULL) {
        return 1; // Succes
    }
    return 0; //failure, server responded but returned error
}