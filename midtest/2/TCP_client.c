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
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(SERVER_ADDRESS, "7777", &hints, &server_addr))
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
    socket_peer = socket(server_addr->ai_family,
                         server_addr->ai_socktype, server_addr->ai_protocol);
    if (socket_peer == -1)
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    if (connect(socket_peer, server_addr->ai_addr, server_addr->ai_addrlen))
    {
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(server_addr);
    printf("Connected.\n\n");

    printf("Sending from me: ");
    while (fgets(sendline, MAXLINE, stdin) != NULL)
    {
        send(socket_peer, sendline, strlen(sendline), 0);
        bzero(buffer, sizeof(buffer));
        int bytes_received = recv(socket_peer, buffer, 1024, 0);
        printf("Receiving from server: ");
        puts(buffer);
        printf("Sending from me: ");
    }

    close(socket_peer);
    printf("Finished.\n");
    return 0;
}
