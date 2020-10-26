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
#include <stdlib.h>

int main()
{
    // DEFINING VARIABLE
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct sockaddr_storage client_address;
    int socket_listen;
    char buf[MAXLINE];
    int n = 0;
    int socket_client;
    printf("********** STARTING SERVER **********\n\n");
    printf("Configuring local address...\n");
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6; //Dual Stack
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(0, "7777", &hints, &servinfo);

    printf("Creating socket ...\n");
    socket_listen = socket(servinfo->ai_family, servinfo->ai_socktype, 0);
    if (socket_listen == -1)
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    int option = 0;
    if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&option,
                   sizeof(option)))
    {
        fprintf(stderr, "setsockopt() failed. (%d)\n", GETSOCKETERRNO());
        return 1; //Dual Stack
    }

    printf("Binding socket to local address...\n");
    if (bind(socket_listen, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Listening ...\n");
    if (listen(socket_listen, 10) == -1)
    {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("\n********** SERVER STARTED **********\n\n");
    printf("=> Waiting for client connection...\n");

    // FD_SET
    fd_set our_sockets;
    FD_ZERO(&our_sockets);
    FD_SET(socket_listen, &our_sockets);

    // FIND MAX SOCKET
    int max_socket;
    max_socket = socket_listen;
    int count = 0;

    // Client Number
    int client_socket_number[3];
    int numbers[3];

    int successfully_received_number = 0;
    int result_number;
    int client_won = 0;
    // SELECT
    for (;;)
    {
        fd_set reads;
        reads = our_sockets;
        if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }
        int i;
        for (i = 1; i <= max_socket; ++i)
        {
            if (FD_ISSET(i, &reads))
            {
                if (i == socket_listen)
                {
                    count++;
                    printf("********** Received request from a new client **********\n");
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    int socket_client = accept(socket_listen,
                                               (struct sockaddr *)&client_address,
                                               &client_len);
                    if (socket_client == -1)
                    {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                GETSOCKETERRNO());
                        return 1;
                    }

                    // 3 Client can connect to one time.
                    if (count <= 3)
                    {
                        FD_SET(socket_client, &our_sockets);
                        if (socket_client > max_socket)
                            max_socket = socket_client;
                        char address_buffer[100];
                        getnameinfo((struct sockaddr *)&client_address,
                                    client_len,
                                    address_buffer, sizeof(address_buffer), 0, 0,
                                    NI_NUMERICHOST);
                        printf("=> New connection from %s\n", address_buffer);
                    }
                    else
                    {
                        printf("=> !!!This server cannot accept this connection (Socket = %d)!!!\n\n", socket_client);
                        close(socket_client);
                        count--;
                    }
                }
                else
                {
                    // IF 3 CLIENT CONNECTION -> CLIENT CAN SEND A NUBMER.
                    if (count == 3)
                    {
                        // RECEIVED NUMBER FROM CLIENT
                        char read[1024];
                        int bytes_received = recv(i, read, 1024, 0);
                        printf("=> Number received from the client (Socket = %d): %s\n\n", i, read);
                        strtok(read, "\n");

                        // STORE A NUMBER FROM CLIENT
                        client_socket_number[successfully_received_number] = i;
                        numbers[successfully_received_number] = atoi(read);

                        // SEND TO CLIENT
                        char result[100];
                        sprintf(result, "We have received your number %d, Please waiting for other client just a moment :)\n", numbers[successfully_received_number]);
                        send(i, result, strlen(result), 0);
                        bzero(read, 1024);

                        ++successfully_received_number;
                    }
                }
            }
        }
        // If received 3 number from each client
        if (successfully_received_number == 3)
        {
            printf("=> All client have been sending number ;)\n");
            printf("=> Generating prize number...\n");

            // RANDOM RESULT PRIZE NUMBER.
            srand(time(0));
            result_number = rand() % 100;
            // FIX HUAY
            //result_number = 88;
            printf("=> Result prize number is %d\n", result_number);
            int j;
            // Cal client won
            for (j = 0; j < 3; j++)
            {
                if (numbers[j] == result_number)
                {
                    client_won++;
                }
            }
            // Announcement of results
            for (j = 0; j < 3; j++)
            {
                char result[1024] = "";
                sprintf(result, "All number is %d %d %d\n", numbers[0], numbers[1], numbers[2]);

                // if client won
                if (numbers[j] == result_number)
                {
                    strcat(result, "You are Winner, get your money.\n");
                }
                else
                {
                    strcat(result, "You're never a Loser until you quit trying.\n");
                }

                // if someone won
                if (client_won > 0)
                {
                    char din_tmp[1024] = "";
                    sprintf(din_tmp, "This time, have %d lucky guys :)\n", client_won);
                    strcat(result, din_tmp);
                }
                else
                {
                    strcat(result, "This time, no one won the prize :(\n");
                }

                send(client_socket_number[j], result, strlen(result), 0);
            }
            printf("\n********** Closing listening socket **********\n\n");
            close(socket_listen);

            printf("Server Finished.\n");
            return 0;
        }
    }
    printf("Closing listening socket...\n");
    close(socket_listen);

    printf("Finished.\n");
    return 0;
}
