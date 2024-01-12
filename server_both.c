#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT1 8080
#define PORT2 8081

int main(int argc, char const *argv[]) {
    int server_fd1, server_fd2, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptors for port 8080
    if ((server_fd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Creating socket file descriptors for port 8081
    if ((server_fd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8081
    if (setsockopt(server_fd2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Server address configuration for port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT1);

    // Binding socket to port 8080
    if (bind(server_fd1, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed on port 8080");
        exit(EXIT_FAILURE);
    }

    // Server address configuration for port 8081
    address.sin_port = htons(PORT2);

    // Binding socket to port 8081
    if (bind(server_fd2, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed on port 8081");
        exit(EXIT_FAILURE);
    }

    // Listening on both ports
    if (listen(server_fd1, 3) < 0 || listen(server_fd2, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Accept connections on port 8080
        if ((new_socket = accept(server_fd1, (struct sockaddr *)&address, &addrlen)) < 0) {
            perror("accept on port 8080");
            continue; // Continue to the next iteration to accept a new connection
        }

        while (1) {
            valread = read(new_socket, buffer, 1024 - 1);

            if (valread <= 0) {
                // Handle the case where the client has closed the connection
                perror("Client closed the connection on port 8080");
                break;
            }

            printf("%s\n", buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent on port 8080\n");
        }

        // Close the connected socket on port 8080
        close(new_socket);

        // Accept connections on port 8081
        if ((new_socket = accept(server_fd2, (struct sockaddr *)&address, &addrlen)) < 0) {
            perror("accept on port 8081");
            continue; // Continue to the next iteration to accept a new connection
        }

        while (1) {
            valread = read(new_socket, buffer, 1024 - 1);

            if (valread <= 0) {
                // Handle the case where the client has closed the connection
                perror("Client closed the connection on port 8081");
                break;
            }

            printf("%s\n", buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent on port 8081\n");
        }

        // Close the connected socket on port 8081
        close(new_socket);
    }

    // The server will not reach this point unless there's an error
    // Closing the listening sockets
    close(server_fd1);
    close(server_fd2);
    return 0;
}
