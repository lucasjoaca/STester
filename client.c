#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "structs.h" 



//Linux-Socket-Tester-Multi-threaded-Network-Service-Diagnostics-Tool
#define SERVER_IP "127.0.0.1" //serverulfacultatii 
#define SERVER_PORT 5000

void send_command_and_print_res(Command *cmd) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    char response_buffer[4096] = {0}; // Buffer mare pentru log-uri

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Conexiune esuata la server");
        return;
    }

    send(client_socket, cmd, sizeof(Command), 0);
    ssize_t bytes = recv(client_socket, response_buffer, 4095, 0);
    
    if (bytes > 0) {
        response_buffer[bytes] = '\0';
        printf("\n[SERVER]: %s\n", response_buffer);
    }
    close(client_socket);
}

int main() {
    int choice;
    Command cmd;

    while (1) {
        printf("\n======= STester: Network Diagnostic Tool =======");
        printf("\n--- HTTP TESTS ---");
        printf("\n  1. HTTP Latency (Ping)");
        printf("\n  2. HTTP Connect (Handshake check)");
        printf("\n  3. HTTP GET Page (Full request)");
        printf("\n--- FTP TESTS ---");
        printf("\n  4. FTP Latency");
        printf("\n  5. FTP Connect");
        printf("\n  6. FTP Create Directory (MKD)");
        printf("\n  7. FTP Upload File (STOR)");
        printf("\n--- OTHER ---");
        printf("\n  8. ECHO Test (Integrity check)");
        printf("\n--- EXECUTION ---");
        printf("\n  9. START SUITE (Run all added tests)");
        printf("\n 10. GET LOGS");
        printf("\n 11. SERVER STATUS");
        printf("\n  0. Exit");
        printf("\nChoice: ");
        
        if (scanf("%d", &choice) != 1) break;
        getchar(); // consume newline

        if (choice == 0) break;
        memset(&cmd, 0, sizeof(Command));

        // Pentru optiunile 1-8, adaugam un test in suita
        if (choice >= 1 && choice <= 8) {
            cmd.cmd = CMD_ADD_TEST;
            TestTarget t;
            memset(&t, 0, sizeof(TestTarget));
            
            printf("Enter Test ID (ex: Test1): "); scanf("%s", t.id);
            printf("Target IP Address: "); scanf("%s", t.adress);
            printf("Target Port: "); scanf("%d", &t.port);

            switch(choice) {
                case 1: t.type = HTTP_LATENCY; break;
                case 2: t.type = HTTP_CONNECT; break;
                case 3: t.type = HTTP_GET_PAGE; break;
                case 4: t.type = FTP_LATENCY; break;
                case 5: t.type = FTP_CONNECT; break;
                case 6: 
                    t.type = FTP_CREATE_DIR; 
                    printf("Credentials (user:pass:dir_name): "); scanf("%s", t.params);
                    break;
                case 7: 
                    t.type = FTP_UPLOAD; 
                    printf("Credentials (user:pass:file_name): "); scanf("%s", t.params);
                    break;
                case 8: 
                    t.type = ECHO_TEST; 
                    printf("Message to echo: "); scanf("%s", t.params);
                    break;
            }
            memcpy(cmd.payload, &t, sizeof(TestTarget));
        } 
        else if (choice == 9) cmd.cmd = CMD_START_SUITE;
        else if (choice == 10) cmd.cmd = CMD_GET_LOGS;
        else if (choice == 11) cmd.cmd = CMD_STATUS;

        send_command_and_print_res(&cmd);
    }
    return 0;
}