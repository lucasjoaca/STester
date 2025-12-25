#define _POSIX_C_SOURCE 199309L // for CLOCK_MONOTONIC
#include "latency_tester.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

double measure_latency(const char* address, int port) {
    struct timespec start, end;
    struct sockaddr_in target_addr;
    int test_socket;

    //socket TCP
    test_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (test_socket < 0) return -1.0;

     struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(test_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    //if i try to test a server that does not exists, i dont want my thread to wait for 75 seconds(default val)
    //so i change it with setsockopt
    
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &target_addr.sin_addr) <= 0) {
        close(test_socket);
        return -1.0;
    }

    
    clock_gettime(CLOCK_MONOTONIC, &start);

    
    if (connect(test_socket, (struct sockaddr*)&target_addr, sizeof(target_addr)) < 0) {
        close(test_socket);
        return -1.0; 
    }

    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    
    close(test_socket);

    double seconds = (double)(end.tv_sec - start.tv_sec);
    double nanoseconds = (double)(end.tv_nsec - start.tv_nsec);
    
    return (seconds * 1000.0) + (nanoseconds / 1000000.0);
}