
#include <stdio.h>
#include "latency_tester.h"
#include "http_tester.h"

int main() {
    const char* ip_test = "127.0.0.1"; 
    int port_http = 5000;

    printf("--- TESTARE UNITARA MOTOARE ---\n");

    // 1. Test Latency
    printf("Masurare latenta pentru %s:%d...\n", ip_test, port_http);
    double lat = measure_latency(ip_test, port_http);
    if (lat >= 0) {
        printf("[OK] Latenta: %.2f ms\n", lat);
    } else {
        printf("[FAIL] Serverul nu raspunde.\n");
    }

    // 2. Test HTTP
    printf("\nVerificare serviciu HTTP pe google.com (142.250.187.174):80...\n");
    int http_res = run_http_test("142.250.187.174", 80); 
    if (http_res == 1) {
        printf("[OK] HTTP Status: 200 OK detectat.\n");
    } else {
        printf("[FAIL] HTTP Error sau timeout.\n");
    }

    return 0;
}