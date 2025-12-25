#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "structs.h" 


#define SERVER_IP "10.100.0.30" //serverulfacultatii 
#define SERVER_PORT 5000




int main(){
    int client_socket;
    struct sockaddr_in server_addr;
    char response_buffer[1024] = {0};

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket < 0){
        perror("ERROR when creating client socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0){
        perror("INET_PTON error: cant find server address");
        close(client_socket);
        return 1;
    }

    printf("Conenected, sending info...\n");
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR: failed to connect to server");
        close(client_socket);
        return 1;
    }
    printf("Client connected to: %s:%d\n", SERVER_IP, PORT);
    Command cmd_send;

    cmd_send.cmd = CMD_STATUS;
    strncpy(cmd_send.payload, "TESTEZ PENTRU DEMO", MAX_PARAMS_LEN - 1);

    cmd_send.payload[MAX_PARAMS_LEN - 1] = '\0';
    if (send(client_socket, &cmd_send, sizeof(Command), 0) < 0) {
        perror("Client: Cant sned command");
        close(client_socket);
        return 1;
    }

    ssize_t bytes_read = recv(client_socket, response_buffer, 1023, 0);
    
    if (bytes_read > 0) {
        response_buffer[bytes_read] = '\0';
        printf("\n--- Server response received ---\n");
        printf("%s\n", response_buffer);
        printf("-----------------------------\n");
    } else if (bytes_read == 0) {
        printf("Server disconnected before sending answer.\n");
    } else {
        perror("Client: Can't receive response");
    }

    close(client_socket);
    return 0;
}