#ifndef ECHO_TESTER_H
#define ECHO_TESTER_H

/**
 * @brief Echo service test (RFC 862).
 * * This function sends a specific string to the server and waits 
 * for the server to send back the exact same data.
 * * @param address The IP address of the Echo server.
 * @param port The port (Standard is 7, but can be any).
 * @return  1 if the echoed data matches exactly, 0 if data mismatch or no data has been sent, -1 for connection error.
 */
// In Testers/echo_tester.h
int run_echo_test(const char* address, int port, const char* message, char* recv_buffer, size_t buffer_size);
#endif