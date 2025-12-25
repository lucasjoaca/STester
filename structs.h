#pragma once


#include <stdbool.h>
#include<time.h>

#define PORT 5000
#define MAX_PARAMS_LEN 1024
#define MAX_ADR_LEN 128



enum TestType{
    HTTP_LATENCY,
    HTTP_CONNECT,
    HTTP_GET_PAGE,
    FTP_LATENCY,
    FTP_CREATE_DIR,
    FTP_CONNECT,
    FTP_UPLOAD,
    ECHO_TEST
    
};


typedef struct structs
{
    char id[128];    //here i store the name of the test
    char adress[MAX_ADR_LEN]; // IP / HOSTNAME
    int port;
    enum TestType type;
    char params[MAX_PARAMS_LEN];
} TestTarget;


//commands that the client send to the server

enum ServerCommand{
    CMD_ADD_TEST,
    CMD_START_SUITE,
    CMD_GET_LOGS, //ask for .log file
    CMD_STATUS,
    CMD_EXIT,
    CMD_INVALID
};

typedef struct{
    enum ServerCommand cmd;
    char payload[4096];
} Command;


typedef struct{
    char test_id[128];
    time_t timestamp;
    double latency; // in miliseconds
    bool is_success;
    char details[1024];
}TestResult;

