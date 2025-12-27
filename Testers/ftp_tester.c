#include "ftp_tester.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int run_ftp_connect_test(const char* address, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr; 
    char buffer[1024] = {0};

    serv_addr.sin_family = AF_INET; //IPv4 addresses
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, address, &serv_addr.sin_addr);
    
    //establish TCP connection
    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        return -1;
    }

    int valread = recv(sock, buffer, 1024, 0);
    close(sock);


    if(valread > 0 && strstr(buffer, "220") != NULL){
        return 1; // Succes
    }
    return 0;
}



int run_ftp_create_dir_test(const char* address, int port, const char* credentials){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    char buffer[1024];

    char user[64], password[64], dir_name[64];

    /**
     * we will parse credentials using sscanf
     * we can do it like this
     * %x[^:]%y[^:]
     * where x is the number of octets and whats in between square brackets means read all except for ":"
     * when it stumbles upon a ":" it will just consume it
     */

     if(sscanf(credentials, "%63[^:]:%63[^:]:%63s", user, password, dir_name) < 3) {
        return 0;
     }
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_port = htons(port);
     inet_pton(AF_INET, address, &serv_addr.sin_addr);

     if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
     }

     //220 signal means its  ok
     int greet = recv(sock, buffer, 1023, 0);
     if (greet <= 0) { 
        close(sock); 
        return 0; 
    }
     buffer[greet] = '\0';
     if(strncmp(buffer, "220", 3) != 0) {
        close(sock);
        perror("Error when trying to establish connection");
        return 0;
     }
     //sending login info

     //for USER we need 331

     sprintf(buffer, "USER %s\r\n", user);
     send(sock, buffer, strlen(buffer), 0);
     int ok = recv(sock, buffer, 1023, 0);
     if (ok <= 0) { 
        close(sock); 
        return 0; 
    }
     buffer[ok] = '\0';
     if(strncmp(buffer, "331", 3) != 0){
        close(sock);
        perror("Error when trying to send user");
        return 0;
     }
     sprintf(buffer, "PASS %s\r\n", password);
     send(sock, buffer, strlen(buffer), 0);
     ok = recv(sock, buffer, 1023, 0);
     if (ok <= 0) { 
        close(sock); 
        return 0; 
    }
     buffer[ok] = '\0';
     if(strncmp(buffer, "230", 3) != 0){
        close(sock);
        perror("Error when trying to send password");
        return 0;
     }
     
     
     sprintf(buffer, "MKD %s\r\n", dir_name);
     send(sock, buffer, strlen(buffer), 0);
     ok = recv(sock, buffer, 1023, 0);
     if (ok <= 0) { 
        close(sock); 
        return 0; 
    }
     buffer[ok] = '\0';
     if(strncmp(buffer, "257", 3) != 0){
        close(sock);
        perror("Error when trying to execute MKD");
        return 0;
     }
     return 1;
}




   /** //! RFC959
     * 3.3.  DATA CONNECTION MANAGEMENT

      Default Data Connection Ports:  All FTP implementations must
      support use of the default data connection ports, and only the
      User-PI may initiate the use of non-default ports.

      Negotiating Non-Default Data Ports:   The User-PI may specify a
      non-default user side data port with the PORT command.  The
      User-PI may request the server side to identify a non-default
      server side data port with the PASV command.  Since a connection
      is defined by the pair of addresses, either of these actions is
      enough to get a different data connection, still it is permitted
      to do both commands to use new ports on both ends of the data
      connection.
      
     *
     DATA PORT (PORT)

            The argument is a HOST-PORT specification for the data port
            to be used in data connection.  There are defaults for both
            the user and server data ports, and under normal
            circumstances this command and its reply are not needed.  If
            this command is used, the argument is the concatenation of a
            32-bit internet host address and a 16-bit TCP port address.
            This address information is broken into 8-bit fields and the
            value of each field is transmitted as a decimal number (in
            character string representation).  The fields are separated
            by commas.  A port command would be:

               PORT h1,h2,h3,h4,p1,p2

            where h1 is the high order 8 bits of the internet host
            address.

         PASSIVE (PASV)

            This command requests the server-DTP to "listen" on a data
            port (which is not its default data port) and to wait for a
            connection rather than initiate one upon receipt of a
            transfer command.  The response to this command includes the
            host and port address this server is listening on. 
     */



int run_ftp_upload_test(const char* address, int port, const char* credentials) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    char buffer[1024];
    char user[64], password[64], filename[64];

    if(sscanf(credentials, "%63[^:]:%63[^:]:%63s", user, password, filename) < 3) {
        return 0;
     }
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_port = htons(port);
     inet_pton(AF_INET, address, &serv_addr.sin_addr);

     if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
     }

     //login is the same as MKD
     int greet = recv(sock, buffer, 1023, 0);
     if (greet <= 0) { 
        close(sock); 
        return 0; 
    }
     buffer[greet] = '\0';
     if(strncmp(buffer, "220", 3) != 0) {
        close(sock);
        perror("Error when trying to establish connection");
        return 0;
     }


     //USER
     sprintf(buffer, "USER %s\r\n", user);
     send(sock, buffer, strlen(buffer), 0);
     int ok = recv(sock, buffer, 1023, 0);
     if (ok <= 0) { 
        close(sock); 
        return 0; 
    }
     buffer[ok] = '\0';
     if(strncmp(buffer, "331", 3) != 0){
        close(sock);
        perror("Error when trying to send user");
        return 0;
     }


     //PASSWORD
     sprintf(buffer, "PASS %s\r\n", password);
     send(sock, buffer, strlen(buffer), 0);
     ok = recv(sock, buffer, 1023, 0);
     if (ok <= 0) { 
        close(sock); 
        return 0; 
    }
     buffer[ok] = '\0';
     if(strncmp(buffer, "230", 3) != 0){
        close(sock);
        perror("Error when trying to send password");
        return 0;
     }
    /**
      * now we have to enter Passive mode

     We cant use Active mode because the client waits the server to connect 
     and do its thing but firewalls usually block external connections

     its like a kid that wants to take candies from a stranger and gives him his Home address, and he is waiting for the stranger,
     but the firewall(his parents) wont let him do that because they dont know that person and assume he might be dangerous
      
     Passive mode works like this:

     The kid asks again for candies but this time he also asks the stranger where can he find him
     The stranger tells him then his address and the kid goes there and gets his candies
     The parents(Firewall) will let him because he wants to go outside, the kid is allowed to go outside
     this is called outbound connection
     

     Firewalls usually dont allow connections from outside, but any connection from inside can go outside
     */

     send(sock, "PASV\r\n", 6, 0);
     ok = recv(sock, buffer, 1023, 0);
     if (ok <= 0) { 
        close(sock); 
        return 0; 
    }
    buffer[ok] = '\0';
    if(strncmp(buffer, "227", 3) != 0) {
        close(sock);
        perror("Error when trying to get Passive Port");
        return 0;
    }
    int h1, h2, h3,  h4, p1, p2; /**
                                *in passive mode we get the Ip address as 4 number, x.y.z.w
                                * and the port as p1, p2, where p1 and p2 is an octet
                                * to calculate the port we use p1 * 256 + p2*/
    char *start = strchr(buffer, '(');
    if(!start || sscanf(start + 1 , "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) < 6) {
        close(sock);
        perror("Error when reading passive port");
        return 0;
    }
    int passive_port = (p1 * 256) + p2;

    int passive_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in passive_addr = serv_addr; // Same IP
    passive_addr.sin_port = htons(passive_port);
    if(connect(passive_sock, (struct sockaddr*)&passive_addr, sizeof(passive_addr)) < 0){
        close(passive_sock);
        close(sock);
        perror("Error when trying to connect to the Data Channel");
        return -1;
    }

    //upload command
    sprintf(buffer, "STOR %s\r\n", filename);
    send(sock, buffer, strlen(buffer), 0);
    ok = recv(sock, buffer, 1023, 0);
    if (ok <= 0) { 
        close(sock); 
        close(passive_sock);
        return 0; 
    }
    char *dummy_content = "STester Network Diagnostic Tool - Upload Test Data\n";
    send(passive_sock, dummy_content, strlen(dummy_content), 0);

    close(passive_sock);

    ok = recv(sock, buffer, 1023, 0);
    if (ok <= 0) { 
        close(sock); 
        return 0; 
    }
     buffer[ok] = '\0';
    close(sock);
    return (strncmp(buffer, "226", 3) == 0);
    /**
     * 
     * we use 2 channel of communication because the server(the one that receives commands from Client.c), who is a FTP Client,
     * asks the FTP server where can it send its files, through which channel
     * and the FTP server then gives him a Data Channel
     
     */

     
}
