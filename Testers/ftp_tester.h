#ifndef FTP_TESTER_H
#define FTP_TESTER_H

/**
 * @brief Runs a test to check if the FTP server is active
 * @param address IP adress of server
 * @param port Port (usually 21 for FTP).
 * @return 1 => succes (220 ), 0 protocol error, -1 offline.
 */
int run_ftp_connect_test(const char* address, int port);


/**
 * @brief FTP test: Create a directory.
 * * Performs full login  and sends the MKD command to check 
 * write permissions on the server.
 * @param address The IP of the server.
 * @param port The FTP port.
 * @param credentials Formatted string as "user:password:directory_name".
 * @return 1 if directory was created (257 code), 0 otherwise.
 */
int run_ftp_create_dir_test(const char* address, int port, const char* user, const char* pass, const char* dirname);
int run_ftp_upload_test(const char* address, int port, int fd);

#endif