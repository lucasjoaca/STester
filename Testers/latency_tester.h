#ifndef LATENCY_TESTER_H
#define LATENCY_TESTER_H

/** * @brief Measures network latency using a TCP Three-Way Handshake.
 * * It starts a TCP connection to the target and uses clock_gettime 
 * to calculate the time elapsed until the connection is established.
 * * @param address The IP address or hostname of the server.
 * @param port The port of the service (e.g., 80, 21).
 * @return double Latency in milliseconds (ms), or -1.0 if server is unreachable.
 */
double measure_latency(const char* address, int port);

#endif