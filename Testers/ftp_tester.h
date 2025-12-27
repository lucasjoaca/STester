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
 * @brief Functional test to create a directory on an FTP server.
 * * This function follows the RFC 959 state machine:
 * 1. Connect to Control Channel (Port 21).
 * 2. Authenticate (USER -> PASS).
 * 3. Send MKD (Make Directory) command.
 * * @param address The IP address of the FTP server.
 * @param port The control port (usually 21).
 * @param credentials String formatted as "user:password:directory_name".
 * @return int 1 for Success, 0 for Failure, -1 for Connection Error.
 */
int run_ftp_create_dir_test(const char* address, int port, const char*);


/**
 * @brief File Upload using Passive Mode (RFC 959).
 * * This test verifies if the server allows file creation and data transfer.
 * It follows a complex state machine:
 * 1. Connect to Control Channel (Port 21) and Authenticate.
 * 2. Send PASV command to receive the Data Port from the server.
 * 3. Parse the (h1,h2,h3,h4,p1,p2) response to calculate the port.
 * 4. Open a second TCP socket (Data Channel) to the calculated port.
 * 5. Send the STOR command on the Control Channel.
 * 6. Send actual data through the Data Channel and close it.
 * 7. Verify the "226 Transfer Complete" response on the Control Channel.
 * * @param address The IP address of the FTP server.
 * @param port The control port (usually 21).
 * @param credentials String formatted as "user:password:filename".
 * @return int 1 for Success, 0 for Protocol Error, -1 for Connection/Network Error.
 */
int run_ftp_upload_test(const char* address, int port, const char* credentials);

#endif