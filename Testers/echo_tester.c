#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
int run_echo_test(const char* address, int port, const char* send_msg,
                  char* recv_buffer, size_t buffer_size) {
   
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, address, &serv_addr.sin_addr);

    // Timeout to prevent hanging
    struct timeval tv;
    tv.tv_sec = 2; 
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        return -1; // Offline
    }

    // Send the user's custom message
    send(sock, send_msg, strlen(send_msg), 0);

    // Receive the reply
    int bytes_received = recv(sock, recv_buffer, buffer_size - 1, 0);
    
    if (bytes_received > 0) {
        recv_buffer[bytes_received] = '\0'; // Null-terminate the string
    } else {
        strcpy(recv_buffer, "NO_RESPONSE");
    }

    close(sock);
    return 1; // Connection was successful
}