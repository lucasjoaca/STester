#include "structs.h"
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include<pthread.h>
#include <stdint.h>
#include "latency_tester.h"
#include "http_tester.h"
#define MAX_RESP 1024
#define NO_OF_TESTS 100



int test_counter = 0;
TestTarget tests_suite[100];
pthread_mutex_t suite_lock = PTHREAD_MUTEX_INITIALIZER; // a mutex that assures us that only one thread can  acces a shared resource at a time, while the others wait

pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER; // a mutex for the log file

void write_to_log(const char* message){
    pthread_mutex_lock(&log_lock);

    FILE* f = fopen("logs.txt", "a");
    if(f == NULL) {
        perror("Error opening log file");
        pthread_mutex_unlock(&log_lock);
        return;
    }

    time_t when_ran = time(NULL); // when was the test ran
    char * timestamp = ctime(&when_ran);

    fprintf(f, "[%s]: %s\n", timestamp, message);
    fprintf(f, "-------------------------------------------------\n");
    
    fclose(f);
    pthread_mutex_unlock(&log_lock);
}


void* handle_client(void *socket_desc){
    int client_socket_fd = (int)(intptr_t) socket_desc;

    char buffer[sizeof(Command)];
    pthread_t  thread_id = pthread_self();
    int bytes_received;
    
    unsigned long tid = (unsigned long)thread_id;
    bytes_received = recv(client_socket_fd, buffer, sizeof(Command), 0);


    if(bytes_received == 0) {
        printf("Nothing packets have been send, client has disconnected!\n");
    }    
    else if(bytes_received < 0) {
        perror("[secv]: error cannot read");
    }
    else {
        Command received_command;
        memcpy(&received_command, buffer, sizeof(Command));
        
        char response[MAX_RESP] = {0};

        switch(received_command.cmd){
            case CMD_ADD_TEST:{
                pthread_mutex_lock(&suite_lock); // block access for other threads
                
                if(test_counter < NO_OF_TESTS) {
                    TestTarget new_test;
                    memcpy(&new_test, received_command.payload, sizeof(TestTarget));

                    tests_suite[test_counter++] = new_test;
                    
                    sprintf(response, "SUCCESS: Test %s added, no of tests in suite: %d", new_test.id, test_counter);
                }
                else {
                    sprintf(response, "ERROR: Test Suite is full!");

                }
                pthread_mutex_unlock(&suite_lock);
                break;
            }
            case CMD_START_SUITE:

                pthread_mutex_lock(&suite_lock);
                
                char log_entry[MAX_RESP] = "REZULTATE:\n"; 
                char line[256];


                for(int i = 0 ; i < test_counter; i++){
                    if(tests_suite[i].type == HTTP_LATENCY){    
                        double lat = measure_latency(tests_suite[i].adress, tests_suite[i].port);
                        int http_ok = run_http_test(tests_suite[i].adress, tests_suite[i].port);

                        if(lat >= 0 && http_ok == 1) {
                            sprintf(line, "Test %s: SUCCESS (Lat: %.2f ms, HTTP 200 OK)\n", tests_suite[i].id, lat);
                        } else if (lat >= 0 && http_ok == 0) {
                            sprintf(line, "Test %s: PARTIAL (Server UP, but HTTP Error)\n", tests_suite[i].id);
                        } else {
                            sprintf(line, "Test %s: FAILED (Offline)\n", tests_suite[i].id);
                        }
                    
          
                    if(strlen(log_entry) + strlen(line) < MAX_RESP - 1) {
                        strcat(log_entry, line);
                    }
                
                    }
                }
                pthread_mutex_unlock(&suite_lock);
                    
                write_to_log(log_entry);
                strncpy(response, log_entry, MAX_RESP - 1);
                
                break;
            case CMD_GET_LOGS:
                sprintf(response, "Comanda GET_LOGS!!!");
                break;
            case CMD_EXIT:
                sprintf(response, "CMD_EXIT, plec");
                break;
            case CMD_STATUS:
                sprintf(response, "CMD_STATUS: %lu", tid);
                break;
            case CMD_INVALID:
            default:
                sprintf(response, "ERROR: comanda este invalida");
                break;
        }
        printf("Received command: %d \n Response sent: %s\n", received_command.cmd, response);
        if(send(client_socket_fd, response, strlen(response), 0) < 0){
            perror("Error when sending info to client");
        }

    }
    close(client_socket_fd);
    printf("[Thread %lu] has done its work.\n", tid);
    return NULL;


}


int setup_server_socket(){
    int listen_socket;
    struct sockaddr_in server_addr;
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(listen_socket == - 1){
        perror("[Setup_server_socket]: error creating socket");
        return -1;
    }
    

    server_addr.sin_family = AF_INET; // Adress famiily internet, folosim IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Can get conn req from anywhere so wew use ANY

    server_addr.sin_port = htons(PORT);

    if(bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error!");
        close(listen_socket);
        return -1;
    }
    if(listen(listen_socket, 3) < 0) {
        perror("listen error");
        close(listen_socket);
        return -1;
    }
    return listen_socket;
}



int main(){
    int listen_socket, client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    listen_socket = setup_server_socket();
    if(listen_socket < 0){
        return 1;
    }
    printf("Serverul STester asculta pe portul:%d...\n", PORT);

    while(1){
        printf("Waiting for conn req...\n");
        
        //! accept() -> waits till a client connects
        client_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &client_len);
        if(client_socket < 0) {
            perror("ERROR: client_socket accept()");
            continue;
        }

        pthread_t client_thread;

        if(pthread_create(&client_thread, NULL, handle_client, (void*)(intptr_t)client_socket) < 0){
            perror("pthread_create: Can't create thread");
            close(client_socket);
            continue;
        }

        pthread_detach(client_thread);

    }
    return 0;
}
