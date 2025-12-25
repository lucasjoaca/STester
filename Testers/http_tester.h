#ifndef HTTP_TESTER_H
#define HTTP_TESTER_H




/**
 * @brief HTTP download test (GET method).
 * * Requests the root page (/) and checks if the server responds 
 * with the "200 OK" status code.
 * * @return 1 for Success, 0 for Application Error (404, 500, etc.).
 */
int run_http_get_page_test(const char* address, int port);

/**
 * @brief Minimal HTTP connection test (HEAD method).
 * * Sends a HEAD request to check if the web server is alive without 
 * downloading the whole page.
 * * @return 1 if server sent a valid HTTP header, 0 otherwise.
 */
int run_http_connect_test(const char* address, int port);
#endif