#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define GETSOCKETERRNO() (errno)
#define SERVER_ADDRESS "127.0.0.1"

#include <stdio.h>
#include <string.h>
#include <time.h>

int main()
{
    struct addrinfo hints;
    struct addrinfo hints2;
    struct addrinfo *server_addr;
    struct addrinfo *server_addr2;
    struct sockaddr_storage server_address;
    int socket_peer;
    const char *message = "Pontep Thaweesup";
    char *message2 = "Hi again";
    char *port1 = "8080";

    printf("Configuring remote address...\n");
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(SERVER_ADDRESS, port1, &hints, &server_addr))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Creating socket...\n");
    socket_peer = socket(server_addr->ai_family, server_addr->ai_socktype, server_addr->ai_protocol);
    if (socket_peer == -1)
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    // send
    printf("Sending: %s\n", message);
    int bytes_sent = sendto(socket_peer, message, strlen(message), 0, server_addr->ai_addr, server_addr->ai_addrlen);
    printf("Sent %d bytes.\n", bytes_sent);

    printf("========================================\n");

    // receive
    socklen_t server_len = sizeof(server_address);
    char read[1024];
    int bytes_received = recvfrom(socket_peer, read, 1024, 0, (struct sockaddr *)&server_address, &server_len);
    printf("Received (%d bytes): %.*s\n", bytes_received, bytes_received, read);
    read[bytes_received] = '\0';
    close(socket_peer);

    // connect to new port
    memset(&hints2, 0, sizeof(hints2));
    hints2.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(SERVER_ADDRESS, read, &hints2, &server_addr2))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Creating new socket for PORT: %s\n", read);
    socket_peer = socket(server_addr2->ai_family, server_addr2->ai_socktype, server_addr2->ai_protocol);
    if (socket_peer == -1)
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    // send
    printf("Sending: %s\n", message2);
    bytes_sent = sendto(socket_peer, message2, strlen(message2), 0, server_addr2->ai_addr, server_addr2->ai_addrlen);
    printf("Sent %d bytes.\n", bytes_sent);

    printf("Finished.\n");
    close(socket_peer);
    freeaddrinfo(server_addr);
    return 0;
}
