#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
// method to handle error
#define MAXSZ 1024
void error(char *msg)
{
    perror(msg);
    exit(1);
}

// command line arguments to server
//argc to take number of arguments and argv to take arguments
int main(int argc, char *argv[])
{
    char smsg[MAXSZ];
    time_t t; // not a primitive datatype
    time(&t);
    int sockfd, newsockfd, portno, clilen, pid;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // check for command line arguments
    // first argument is executable name and second is port number
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, not enough arguments\n");
        exit(1);
    }

    // AF_INET = IPv4 and SOCK_STREAM = TCP and 0 = any port
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // check for socket error
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    // bzero() sets all bytes in the structure to 0 and returns a pointer to it
    // so we can use it to set the structure's members
    // clear serv_addr to 0
    bzero((char *)&serv_addr, sizeof(serv_addr));

    // get port number from command line stored at index 1 in argv array
    // atoi() converts a string to an integer
    portno = atoi(argv[1]);

    // set serv_addr.sin_family to AF_INET because we want an IPv4 address
    serv_addr.sin_family = AF_INET;
    // set serv_addr.sin_addr.s_addr to INADDR_ANY because we want to listen on any interface
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // set serv_addr.sin_port to portno on which we want to listen
    // htons() converts a 16-bit integer to network byte order
    serv_addr.sin_port = htons(portno);

    // bind() binds the socket to the port number and interface
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    // listen() puts the socket in listening mode
    // sockfd is the socket descriptor we got from the socket() call and 5 is the maximum number of queued connections that are clients
    listen(sockfd, 5);

    printf("Server listening on port %d\n", portno);

    // clilen is the size of the structure that will be used to store the client's address
    clilen = sizeof(cli_addr);

    while (1)
    {
        // creates a new socket descriptor for the client
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        printf("-----------------------------------------------------\n");
        // printf("Got a connection from %s\n", inet_ntoa(cli_addr.sin_addr));
        printf("++ Client Connected :[%s:%u]", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        // check for accept() error
        if (newsockfd < 0)
        {
            error("ERROR on accept");
        }
        // pid = fork() creates a new process
        pid = fork();
        // check for fork() error
        if (pid < 0)
        {
            error("ERROR on fork");
        }
        pid < 0 ? printf("Error Creating Child\n") : (pid > 0) ? printf(" || Child Process Created with PID= %d\n", pid)
                                                               : printf("\n");

        if (pid == 0)
        {

            while (1)
            {
                // bzero() sets all bytes in the structure to 0 and returns a pointer to it
                bzero(buffer, 256);

                // read() reads up to 256 bytes from the socket and stores them in buffer
                // n is the number of bytes read
                if ((n = recv(newsockfd, buffer, MAXSZ, 0)) <= 0)
                    printf("-- Client Disconnected :[%s:%u]\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port)), exit(1);

                // print the message from the client
                printf("Message from client: %s\n", buffer);
                strcpy(smsg, ctime(&t));
                send(newsockfd, smsg, strlen(smsg), 0);
            }
        }
        else
        {
            close(newsockfd);
        }
    }

    return 0;
}