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
        struct addrinfo *bind_address;
        struct sockaddr_storage client_address;
        int socket_listen;
        char *hello = "Good Job";
        char *port;

        printf("Configuring local address...\n");
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;

        getaddrinfo(0, "8080", &hints, &bind_address);

        printf("Creating socket...\n");
        socket_listen = socket(bind_address->ai_family,
                               bind_address->ai_socktype, bind_address->ai_protocol);
        if (socket_listen == -1)
        {
                fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
                return 1;
        }

        printf("Binding socket to local address...\n");
        if (bind(socket_listen,
                 bind_address->ai_addr, bind_address->ai_addrlen) == -1)
        {
                fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
                return 1;
        }
        freeaddrinfo(bind_address);

        socklen_t client_len = sizeof(client_address);
        char read[1024];
        int bytes_received = recvfrom(socket_listen,
                                      read, 1024,
                                      0,
                                      (struct sockaddr *)&client_address, &client_len);

        printf("Received (%d bytes): %.*s\n",
               bytes_received, bytes_received, read);

        printf("Remote address is: ");
        char address_buffer[100];
        char service_buffer[100];
        getnameinfo(((struct sockaddr *)&client_address),
                    client_len,
                    address_buffer, sizeof(address_buffer),
                    service_buffer, sizeof(service_buffer),
                    NI_NUMERICHOST | NI_NUMERICSERV);
        printf("%s Port: %s\n", address_buffer, service_buffer);

        printf("===================================\n");

        port = "8888";
        printf("Please connect to a new PORT: %s\n", port);

        sendto(socket_listen, port, strlen(port),
               0, (struct sockaddr *)&client_address, client_len);
        close(socket_listen);

        getaddrinfo(0, port, &hints, &bind_address);
        printf("Creating new socket \n");
        socket_listen = socket(bind_address->ai_family,
                               bind_address->ai_socktype, bind_address->ai_protocol);
        if (socket_listen == -1)
        {
                fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
                return 1;
        }

        printf("Binding the new socket to local address...\n");
        if (bind(socket_listen,
                 bind_address->ai_addr, bind_address->ai_addrlen) == -1)
        {
                fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
                return 1;
        }
        freeaddrinfo(bind_address);
        bzero(read, sizeof(read));
        bytes_received = recvfrom(socket_listen,
                                  read, 1024,
                                  0,
                                  (struct sockaddr *)&client_address, &client_len);

        printf("Received (%d bytes): %.*s\n",
               bytes_received, bytes_received, read);

        printf("Finished.\n");
        return 0;
}
