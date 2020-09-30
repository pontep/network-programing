#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#define GETSOCKETERRNO() (errno)
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_ADDRESS6 "::1"
#define MAXLINE 4096
#include <stdio.h>
#include <string.h>
#include <time.h>

int main()
{
    struct addrinfo hints;
    struct addrinfo *server_addr;
    struct sockaddr_storage server_from;
    int socket_peer;
    char sendline[MAXLINE], recvline[MAXLINE];
    char buffer[1024];

    printf("Configuring remote address...\n");
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(SERVER_ADDRESS6, "8888", &hints, &server_addr))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Server address is: ");
    char address_buffer[100];
    char port_buffer[100];
    getnameinfo(server_addr->ai_addr, server_addr->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                port_buffer, sizeof(port_buffer),
                NI_NUMERICHOST | NI_NUMERICSERV);
    printf("%s \nPort is: %s\n", address_buffer, port_buffer);

    printf("Creating socket...\n");
    socket_peer = socket(server_addr->ai_family, server_addr->ai_socktype, server_addr->ai_protocol);
    if (socket_peer == -1)
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    if (connect(socket_peer, server_addr->ai_addr, server_addr->ai_addrlen) == -1)
    {
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    printf("Connected. \n");

    printf("Sending from me: ");
    while (fgets(sendline, MAXLINE, stdin) != NULL)
    {
        // SEND TO SERVER
        send(socket_peer, sendline, strlen(sendline), 0);

        // RECEIVED FROM SERVER
        char read[1024];
        int bytes_received = recv(socket_peer, read, 1024, 0);
        if (bytes_received < 1)
        {
            printf("received error \n\n");
        }
        printf("Received from server: %s\n\n", read);

        strtok(sendline, "\n");
        if (strcmp(sendline, "close") == 0)
        {
            close(socket_peer);
            printf("Finished.\n");
            return 0;
        }
        bzero(read, 1024);
        printf("Sending from me: ");
    }

    close(socket_peer);
    printf("Finished.\n");
    return 0;
}