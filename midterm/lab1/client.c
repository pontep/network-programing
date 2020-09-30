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
    struct addrinfo *server_addr;
    struct sockaddr_storage server_address;
    int socket_peer;
    const char *message = "Pontep Thaweesup";

    printf("Configuring remote address...\n");
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(SERVER_ADDRESS, "9783", &hints, &server_addr))
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

    printf("Finished.\n");
    close(socket_peer);
    freeaddrinfo(server_addr);
    return 0;
}
