#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
// #include <sys/time.h>
#include <asm-generic/socket.h>

// #define PORT 8080

long long get_nanoseconds()
{
    struct timespec current_time;
    clock_gettime(0, &current_time);
    return ((long long)current_time.tv_sec * 1000000000LL) + (long long)current_time.tv_nsec;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Port number not set. Usage %s <port_number>\n", argv[0]);
        return -1;
    }
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    int port = atoi(argv[1]);
    // long nanoseconds;
    // struct timeval current_time;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0)
        {
            perror("accept");
            continue; // Continue to the next iteration to accept a new connection
        }

        while (1)
        {
            valread = read(new_socket, buffer, 1024 - 1);
            printf("Got message at %ld\n", get_nanoseconds());
            if (valread <= 0)
            {
                // Handle the case where the client has closed the connection
                perror("Client closed the connection");
                break;
            }

            printf("Sending reply at %ld\n", get_nanoseconds());
            send(new_socket, hello, strlen(hello), 0);
            // printf("Hello message sent\n");
        }

        // Close the connected socket
        close(new_socket);
    }

    // The server will not reach this point unless there's an error
    // Closing the listening socket
    close(server_fd);
    return 0;
}
