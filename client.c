// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#define PORT 8080

// Signal handler for SIGPIPE
void sigpipe_handler(int signo)
{
    // Do nothing, just ignore the signal
}

int main(int argc, char const *argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};

    const char *serv_addr_ip = argv[1];
    int sleep_usecs = atoi(argv[2]);

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, serv_addr_ip, &serv_addr.sin_addr) <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr,
                          sizeof(serv_addr))) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    signal(SIGPIPE, sigpipe_handler);
    while (1)
    {
        send(client_fd, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        valread = read(client_fd, buffer, 1024 - 1);
        if (valread <= 0)
        {
            // Handle the case where the server closes the connection
            perror("Server closed the connection");
            break;
        }
        printf("%s\n", buffer);
        // Remove or adjust the sleep duration if needed
    }
    // closing the connected socket
    close(client_fd);
    return 0;
}
