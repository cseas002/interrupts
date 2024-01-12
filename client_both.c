#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#define PORT 8080
#define PORT2 8081

// Signal handler for SIGPIPE
void sigpipe_handler(int signo)
{
    // Do nothing, just ignore the signal
}

int send_request(int client_fd, char *hello, char *buffer, int port)
{
    struct timeval start_time, end_time;
    // Measure the time before sending the pre request
    gettimeofday(&start_time, NULL);

    // Send to port 8080
    send(client_fd, hello, strlen(hello), 0);
    // printf("Pre request sent to port 8080\n");

    // Receive from port 8080
    int valread = read(client_fd, buffer, sizeof(buffer));
    if (valread <= 0)
    {
        perror("Error reading from port 8080");
        return -1;
    }
    // Measure the time after sending to port 8080
    gettimeofday(&end_time, NULL);
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);
    // printf("Time taken for send to port %d: %ld microseconds\n", port, elapsed_time);

    // printf("Message from port 8080: %s\n", buffer);
    return elapsed_time;
}

int main(int argc, char const *argv[])
{
    int status, valread, client_fd1, client_fd2;
    struct sockaddr_in serv_addr1, serv_addr2;
    char *hello = "Hello from client";
    char buffer[1024] = {0};

    const char *serv_addr_ip = argv[1];
    int sleep_usecs = atoi(argv[2]);
    int repetitions = atoi(argv[3]);

    // Create socket for port 8080
    if ((client_fd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error for port 8080\n");
        return -1;
    }

    serv_addr1.sin_family = AF_INET;
    serv_addr1.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, serv_addr_ip, &serv_addr1.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to port 8080
    if ((status = connect(client_fd1, (struct sockaddr *)&serv_addr1, sizeof(serv_addr1))) < 0)
    {
        printf("\nConnection Failed to port 8080\n");
        return -1;
    }

    // Create socket for port 8081
    if ((client_fd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error for port 8081\n");
        return -1;
    }

    serv_addr2.sin_family = AF_INET;
    serv_addr2.sin_port = htons(PORT2);

    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, serv_addr_ip, &serv_addr2.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to port 8081
    if ((status = connect(client_fd2, (struct sockaddr *)&serv_addr2, sizeof(serv_addr2))) < 0)
    {
        printf("\nConnection Failed to port 8081\n");
        return -1;
    }

    signal(SIGPIPE, sigpipe_handler);
    long time_taken, total_time = 0;

    for (int i = 0; i < repetitions; i++)
    {
        // Send request to port 8080
        time_taken = send_request(client_fd1, hello, buffer, 8080);
        if (time_taken == -1)
            break;

        usleep(100);

        // Send request to port 8081
        time_taken = send_request(client_fd2, hello, buffer, 8081);
        if (time_taken == -1)
            break;

        total_time += time_taken; // We care only for the second (original request)
        usleep(sleep_usecs);
    }

    printf("Total time taken: %ld\n", total_time);
    // Close the connections (not reached in the current code)
    close(client_fd1);
    close(client_fd2);

    return 0;
}
