#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // sys/socket.h header is needed to use socket() function
#include <sys/types.h>  // sys/types.h header is needed to use SOCK_STREAM
#include <netinet/in.h> // netinet/in.h header is needed to use IPPROTO_TCP and struct sockaddr_in
#include <unistd.h>     // unistd.h header is needed to use close() function
#include <arpa/inet.h>  // inet* headers are needed to convert IP address from string to binary
#include <netdb.h>

// method to handle errors
void error(char *msg)
{
    perror(msg);
    exit(0);
}

// command line arguments to server
//argc to take number of arguments and argv to take arguments
// minimum three arguments to be needed 1. Executable name 2. Server IP 3. Port Number
int main(int argc, char *argv[])
{
    // sockfd to store socket file descriptor, portno to store port number
    int sockfd, portno, n;
    // server_addr to store server IP address
    struct sockaddr_in serv_addr;

    struct hostent *server;

    // buff to store the data received from server
    char buffer[256];

    // if the number of arguments is less than 3, print error message and exit
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    // get the hostname and port number from command line
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // if socket() fails, print error message and exit
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    // gethostbyname is function to get the hostname
    server = gethostbyname(argv[1]);

    // if gethostbyname() fails, print error message and exit
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // bzero clear the server address
    bzero((char *)&serv_addr, sizeof(serv_addr));

    // serv.addr.sin_family is used to set the address family
    serv_addr.sin_family = AF_INET;

    // bcopy is used to copy the host address
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    // serv_addr.sin_port is used to set the port number
    serv_addr.sin_port = htons(portno);

    // connect is used to connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }
    printf("Connected to server\n");

    printf("Enter message to send: ");
    bzero(buffer, 256);
    fgets(buffer, 256, stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        error("ERROR writing to socket");
    }

    // bzero clears the buffer
    bzero(buffer, 256);
    // read() reads the data from the server
    n = read(sockfd, buffer, 255);
    // if read() fails, print error message and exit
    if (n < 0)
    {
        error("ERROR reading from socket");
    }

    // print the data received from server
    printf("Server: %s\n", buffer);
    return 0;
}