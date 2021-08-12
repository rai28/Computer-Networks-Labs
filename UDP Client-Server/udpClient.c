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
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock, length, n;
    struct sockaddr_in server, from;
    struct hostent *host;
    char buffer[256];

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
        exit(1);
    }
    // create socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        error("ERROR opening socket");
    }
    server.sin_family = AF_INET;
    host = gethostbyname(argv[1]);
    if (host == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }
    bcopy((char *)host->h_addr, (char *)&server.sin_addr, host->h_length);
    server.sin_port = htons(atoi(argv[2]));
    length = sizeof(struct sockaddr_in);
    // send message to server
    printf("Enter message to be send: ");
    bzero(buffer, 256);
    fgets(buffer, 256, stdin);
    n = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server, length);
    if (n < 0)
    {
        error("ERROR writing to socket");
    }
    // receive message from server
    n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from, &length);
    if (n < 0)
    {
        error("ERROR reading from socket");
    }
    printf("Message received: %s\n", buffer);
    close(sock);
    return 0;
}
