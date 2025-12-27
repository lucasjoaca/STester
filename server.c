#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include "Testers/latency_tester.h"
#include "Testers/http_tester.h"
#include "Testers/echo_tester.h"
#include "Testers/ftp_tester.h"
#define MAX_RESP 1024
#define NO_OF_TESTS 100

int test_counter = 0;
TestTarget tests_suite[100];
pthread_mutex_t suite_lock = PTHREAD_MUTEX_INITIALIZER; // a mutex that assures us that only one thread can  acces a shared resource at a time, while the others wait

pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER; // a mutex for the log file
/**
 * @brief Writes a message to the "logs.txt" file.
 * * Adds a timestamp to the message and uses a mutex (log_lock)
 * to make sure multiple threads don't write at the same time.
 * * @param message The string containing the test suite results.
 */
void write_to_log(const char *message)
{
    pthread_mutex_lock(&log_lock);

    FILE *f = fopen("logs.txt", "a");
    if (f == NULL)
    {
        perror("Error opening log file");
        pthread_mutex_unlock(&log_lock);
        return;
    }

    time_t when_ran = time(NULL); // when was the test ran
    char *timestamp = ctime(&when_ran);

    fprintf(f, "[%s]: %s\n", timestamp, message);
    fprintf(f, "-------------------------------------------------\n");

    fclose(f);
    pthread_mutex_unlock(&log_lock);
}

void *handle_client(void *socket_desc)
{
    int client_socket_fd = (int)(intptr_t)socket_desc;

    char buffer[sizeof(Command)];
    pthread_t thread_id = pthread_self();
    int bytes_received;

    unsigned long tid = (unsigned long)thread_id;
    bytes_received = recv(client_socket_fd, buffer, sizeof(Command), 0);

    if (bytes_received == 0)
    {
        printf("Nothing packets have been sent, client has disconnected!\n");
    }
    else if (bytes_received < 0)
    {
        perror("[secv]: error cannot read");
    }
    else
    {
        Command received_command;
        memcpy(&received_command, buffer, sizeof(Command));

        char response[4096] = {0};

        switch (received_command.cmd)
        {
        case CMD_ADD_TEST:
        {
            pthread_mutex_lock(&suite_lock); // block access for other threads

            if (test_counter < NO_OF_TESTS)
            {
                TestTarget new_test;
                // Clear the memory just for safety
                memset(&new_test, 0, sizeof(TestTarget));

                memcpy(&new_test, received_command.payload, sizeof(TestTarget));

                tests_suite[test_counter++] = new_test;

                // debugging
                printf("[SERVER] Adaugat test: ID=%s, Tip=%d, Target=%s:%d\n",
                       new_test.id, new_test.type, new_test.adress, new_test.port);

                sprintf(response, "SUCCESS: Test %s added, no of tests in suite: %d", new_test.id, test_counter);
            }
            else
            {
                sprintf(response, "ERROR: Test Suite is full!");
            }
            pthread_mutex_unlock(&suite_lock);
            break;
        }
        case CMD_START_SUITE:

            pthread_mutex_lock(&suite_lock);

            if (test_counter == 0)
            {
                sprintf(response, "ERROR: No tests in suite. Add some tests first!");
                pthread_mutex_unlock(&suite_lock);
                break;
            }

            char log_entry[4096] = "REZULTATE:\n";
            char line[1024];

            for (int i = 0; i < test_counter; i++)
            {
                double lat = measure_latency(tests_suite[i].adress, tests_suite[i].port);
                int success = 0;
                char details[2200] = "";

                if (lat < 0)
                {
                    success = 0;
                    sprintf(details, "OFFLINE: Connection timed out");
                }
                else
                {
                    switch (tests_suite[i].type)
                    {
                    case ECHO_TEST:
                    {
                        char echo_reply[1024] = {0};
                        success = run_echo_test(tests_suite[i].adress, tests_suite[i].port, tests_suite[i].params, echo_reply, sizeof(echo_reply));

                        if (success == 1)
                        {
                            if (strcmp(tests_suite[i].params, echo_reply) == 0)
                            {
                                sprintf(details, "ECHO: Message intact [%s]", echo_reply);
                            }
                            else
                            {
                                success = 0;
                                sprintf(details, "ECHO MISMATCH: Sent [%s], Got [%s]", tests_suite[i].params, echo_reply);
                            }
                        }
                        else
                        {
                            sprintf(details, "ECHO ERROR: Server unreachable");
                        }
                    }
                    break;
                    case HTTP_LATENCY:
                        success = 1;
                        sprintf(details, "HTTP Latency: %.2f ms", lat);
                        break;
                    case HTTP_GET_PAGE:
                        success = run_http_get_page_test(tests_suite[i].adress, tests_suite[i].port);
                        sprintf(details, "Lat: %.2f ms, GET: %s", lat, (success ? "200 OK" : "ERR"));
                        break;
                    case HTTP_CONNECT:
                        success = run_http_connect_test(tests_suite[i].adress, tests_suite[i].port);
                        sprintf(details, "HTTP Connect: %s", (success == 1 ? "READY" : "FAILED"));
                        break;
                    case FTP_CREATE_DIR:
                        success = run_ftp_create_dir_test(tests_suite[i].adress, tests_suite[i].port, tests_suite[i].params);
                        sprintf(details, "FTP MKD: %s", (success == 1 ? "DIRECTORY CREATED" : "PERMISSION DENIED/FAILED"));
                        break;
                    case FTP_UPLOAD:
                        success = run_ftp_upload_test(tests_suite[i].adress, tests_suite[i].port, tests_suite[i].params);
                        sprintf(details, "FTP UPLOAD: %s", (success == 1 ? "UPLOADED SUCCESSFULLY" : "AN ERROR OCCURED AND UPLOAD FAILED"));
                        break;
                    case FTP_LATENCY:
                        success = 1;
                        sprintf(details, "FTP Latency: %.2f ms", lat);
                        break;

                    case FTP_CONNECT:
                        success = run_ftp_connect_test(tests_suite[i].adress, tests_suite[i].port);
                        sprintf(details, "Lat: %.2f ms, FTP: %s", lat, (success ? "READY" : "ERR"));
                        break;
                    default:
                        success = 0;
                        sprintf(details, "Unknown test type");
                    }
                }

                char current_line[2560];
                sprintf(current_line, "ID: %s | %s | [%s]",
                        tests_suite[i].id, details, (success ? "PASSED" : "FAILED"));

                // Scriem imediat rezultatul acestui test in fisier
                write_to_log(current_line);
            }
            write_to_log("--- SFARSIT SUITA TESTE ---\n");
            pthread_mutex_unlock(&suite_lock);
            sprintf(response, "SUCCESS: Suite executed. Check logs for details.");
            test_counter = 0;
            break;
            case CMD_GET_LOGS:
        {
            pthread_mutex_lock(&log_lock);
            
            // a+ bc we want to create the file i ncase it does not exists
            FILE *f = fopen("logs.txt", "a+");
            
            if (f == NULL) {
                sprintf(response, "ERROR: Could not open or create logs.txt");
            } else {

                fseek(f, 0, SEEK_END);
                long size = ftell(f);

                if (size == 0) {
                    sprintf(response, "INFO: Log file is empty.");
                } else {
                    long offset = (size > 2000) ? size - 2000 : 0;
                    fseek(f, offset, SEEK_SET);

                    size_t read_bytes = fread(response, 1, 2000, f);
                    response[read_bytes] = '\0';
                }
                fclose(f);
            }
            
            pthread_mutex_unlock(&log_lock);
            break;
        }
            case CMD_STATUS:
            {
                pthread_mutex_lock(&suite_lock);
                sprintf(response, "STATUS: Server is UP. Suite contains %d/%d tests.", test_counter, NO_OF_TESTS);
                pthread_mutex_unlock(&suite_lock);
                break;
            }
            case CMD_EXIT:
            {
                sprintf(response, "BYE: Closing connection. Server remains active.");
                break;
            }
        case CMD_INVALID:
        default:
            sprintf(response, "ERROR: Unknown command!");
            break;
        }
        printf("Received command: %d \n Response sent: %s\n", received_command.cmd, response);
        if (send(client_socket_fd, response, strlen(response), 0) < 0)
        {
            perror("Error when sending info to client");
        }
    }
    close(client_socket_fd);
    printf("[Thread %lu] has done its work.\n", tid);
    return NULL;
}

int setup_server_socket()
{
    int listen_socket;
    struct sockaddr_in server_addr;
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_socket == -1)
    {
        perror("[Setup_server_socket]: error creating socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;         // Adress famiily internet, folosim IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Can get conn req from anywhere so wew use ANY

    server_addr.sin_port = htons(PORT);

    if (bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind error!");
        close(listen_socket);
        return -1;
    }
    if (listen(listen_socket, 3) < 0)
    {
        perror("listen error");
        close(listen_socket);
        return -1;
    }
    return listen_socket;
}

int main()
{
    int listen_socket, client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    listen_socket = setup_server_socket();
    if (listen_socket < 0)
    {
        return 1;
    }
    printf("Serverul STester asculta pe portul:%d...\n", PORT);
    FILE *f = fopen("logs.txt", "a");
    if (f) {
        fprintf(f, "--- SERVER STARTED ---\n");
        fclose(f);
    }
    while (1)
    {
        printf("Waiting for conn req...\n");

        //! accept() -> waits till a client connects
        client_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0)
        {
            perror("ERROR: client_socket accept()");
            continue;
        }

        pthread_t client_thread;

        if (pthread_create(&client_thread, NULL, handle_client, (void *)(intptr_t)client_socket) < 0)
        {
            perror("pthread_create: Can't create thread");
            close(client_socket);
            continue;
        }

        pthread_detach(client_thread);
    }
    return 0;
}
