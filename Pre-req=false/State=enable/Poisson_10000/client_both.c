#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <math.h>
#include <stdbool.h>
#define PORT 8080
#define PORT2 8081

// Signal handler for SIGPIPE
void sigpipe_handler(int signo)
{
    // Do nothing, just ignore the signal
}

void readParameters(const char *filename, char **serv_addr_ip, int *sleep_usecs, int *repetitions, bool *poisson, bool *pre_request)
{
    FILE *param_file = fopen(filename, "r");
    if (param_file == NULL)
    {
        perror("Error opening parameter file");
        exit(EXIT_FAILURE);
    }

    char param_name[20];
    char param_value[20];
    *poisson = false;
    *pre_request = false;

    // Read parameters from the file
    while (fscanf(param_file, "%s %s", param_name, param_value) == 2)
    {
        if (strcmp(param_name, "server_ip") == 0)
        {
            *serv_addr_ip = strdup(param_value);
        }
        else if (strcmp(param_name, "sleep_usecs") == 0)
        {
            *sleep_usecs = atoi(param_value);
        }
        else if (strcmp(param_name, "repetitions") == 0)
        {
            *repetitions = atoi(param_value);
        }
        else if (strcmp(param_name, "poisson") == 0)
        {
            if (strcmp(param_value, "true") == 0)
            {
                *poisson = true;
            }
            else if (strcmp(param_value, "false") == 0)
            {
                *poisson = false;
            }
        }
        else if (strcmp(param_name, "pre_request") == 0)
        {
            if (strcmp(param_value, "true") == 0)
            {
                *pre_request = true;
            }
            else if (strcmp(param_value, "false") == 0)
            {
                *pre_request = false;
            }
        }
    }

    fclose(param_file);
}

// Function to generate Poisson-distributed random numbers
int poissonRandomNumber(double lambda)
{
    double L = exp(-lambda);
    double p = 1.0;
    int k = 0;

    do
    {
        k++;
        p *= ((double)rand() / RAND_MAX);
    } while (p > L);

    return k - 1;
}

int send_request(bool read_message, int client_fd, char *hello, char *buffer, int port)
{
    struct timeval start_time, end_time;
    // Measure the time before sending the pre request
    gettimeofday(&start_time, NULL);

    // Send to port 8080
    int r = send(client_fd, hello, strlen(hello), 0);
    // printf("Pre request sent to port 8080\n");
    if (r <= 0)
    {
        perror("Error sending to port");
        return -1;
    }

    // Receive from port
    if (read_message)
    {
        int valread = read(client_fd, buffer, 1024);
        if (valread <= 0)
        {
            perror("Error reading from port");
            return -1;
        }
    }

    // Measure the time after sending to port
    gettimeofday(&end_time, NULL);
    long elapsed_time = end_time.tv_usec - start_time.tv_usec;

    // printf("Time taken for send to port %d: %ld microseconds\n", port, elapsed_time);

    // printf("Message from port 8080: %s\n", buffer);
    return elapsed_time;
}

int compare_long(const void *a, const void *b)
{
    return (*(long *)a - *(long *)b);
}

void print_statistics(int repetitions, long *latency_array)
{
    // Calculate and print average latency
    long sum_latency = 0;
    for (int i = 0; i < repetitions; i++)
    {
        sum_latency += latency_array[i];
    }
    double average_latency = (double)sum_latency / repetitions;
    printf("Average Latency: %.2f microseconds\n", average_latency);

    // Sort the latency array to find the 99th percentile
    qsort(latency_array, repetitions, sizeof(long), compare_long);

    // Calculate and print the 99th percentile latency
    int index_99th = (int)(0.99 * repetitions);
    long percentile_99th = latency_array[index_99th];
    printf("99th Percentile Latency: %ld microseconds\n", percentile_99th);
}

int main(int argc, char const *argv[])
{
    int status, valread, client_fd1, client_fd2;
    struct sockaddr_in serv_addr1, serv_addr2;
    bool poisson, pre_request;
    char *hello = "Hello from client";
    char buffer[1024] = {0};

    if (argc != 2)
    {
        printf("Usage: %s <parameter_file>\n", argv[0]);
        return -1;
    }

    char *serv_addr_ip;
    int sleep_usecs;
    int repetitions;

    // Read parameters from the file using the function
    readParameters(argv[1], &serv_addr_ip, &sleep_usecs, &repetitions, &poisson, &pre_request);

    printf("Parameters:\nServer IP address: %s\nSleep microseconds: %d\nRepetitions: %d\nPoisson Distribution: %d\nSend Pre request: %d\n\n\n", serv_addr_ip, sleep_usecs, repetitions, poisson, pre_request);

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

    // Additional variables for latency measurement
    long *latency_array = malloc(repetitions * sizeof(long));
    if (latency_array == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Seed the random number generator for the Poisson distribution
    srand((unsigned int)time(NULL));
    // Sleep for microseconds between one third of sleeping time until three times the sleeping time
    int sleep_usecs_min = sleep_usecs / 3;
    int sleep_usecs_max = sleep_usecs * 3;

    // Use Poisson for the requests inter arrival time
    // Measure tail latency and average time
    for (int i = 0; i < repetitions; i++)
    {
        // Send request to port 8080 without reading it
        if (pre_request)
        {
            time_taken = send_request(false, client_fd1, hello, buffer, 8080);
            if (time_taken == -1)
                break;
        }
        usleep(100); // Sleep for 100 us

        // Send request to port 8081
        time_taken = send_request(true, client_fd2, hello, buffer, 8081);
        if (time_taken == -1)
            break;

        latency_array[i] = time_taken; // Save the time taken in the latency array
        fprintf(stderr, "Time taken: %ld\n", time_taken);
        total_time += time_taken; // We care only for the second (original request)
        // Generate sleep duration based on Poisson distribution
        if (poisson)
        {
            int poissonValue = poissonRandomNumber(1.0); // Adjust lambda as needed
            int sleep_duration = sleep_usecs_min + poissonValue % (sleep_usecs_max - sleep_usecs_min + 1);
            usleep(sleep_duration);
        }
        else
        {
            usleep(sleep_usecs);
        }
    }

    printf("Total time taken: %ld\n", total_time);
    // Close the connections (not reached in the current code)

    print_statistics(repetitions, latency_array);
    close(client_fd1);
    close(client_fd2);
    free(serv_addr_ip);
    free(latency_array);
    return 0;
}
