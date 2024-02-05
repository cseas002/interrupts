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
#include <gsl/gsl_randist.h>
#define PORT 8080
#define PORT2 8081

// Signal handler for SIGPIPE
void sigpipe_handler(int signo)
{
    // Do nothing, just ignore the signal
}

void readParameters(const char *filename, char **serv_addr_ip, int *sleep_usecs, int *repetitions, bool *poisson, bool *exponential, bool *fixed, bool *pre_request, int *pre_request_interval)
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
    *fixed = false;
    *exponential = false;
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
        else if (strcmp(param_name, "distribution") == 0)
        {
            if (strcmp(param_value, "poisson") == 0)
            {
                *poisson = true;
            }
            else if (strcmp(param_value, "fixed") == 0)
            {
                *fixed = true;
            }
            else if (strcmp(param_value, "exponential") == 0)
            {
                *exponential = true;
            }
            else
            {
                fprintf(stderr, "Choose from poisson, exponential, fixed\n");
                exit(EXIT_FAILURE);
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
        else if (strcmp(param_name, "pre_request_interval") == 0)
        {
            *pre_request_interval = atoi(param_value);
        }
    }

    if ((*exponential && *fixed) || (*exponential && *poisson) || (*fixed && *poisson))
    {
        fprintf(stderr, "Cannot combine fixed, exponential, and poisson\nFixed: %d, Poisson: %d, Exponential: %d", *fixed, *poisson, *exponential);
        fprintf(stderr, "Choose from poisson, exponential, fixed\n");
        exit(EXIT_FAILURE);
    }

    if (!(*fixed || *poisson || *exponential))
    {
        fprintf(stderr, "Set one from fixed, poisson and exponential\n");
        fprintf(stderr, "Choose from poisson, exponential, fixed\n");
        exit(EXIT_FAILURE);
    }

    fclose(param_file);
}

// Function to generate random numbers from an exponential distribution
int generateExponential(int average)
{
    if (average == 0)
    {
        return 0;
    }
    double lambda = 1 / (double)average;
    double u = (double)rand() / RAND_MAX;
    return (int)(-log(1 - u) / lambda);
}

int *get_poisson_numbers(int repetitions, int sleep_usecs)
{
    // Construct the command to execute the Python script with parameters
    char command[100];
    snprintf(command, sizeof(command), "python poisson.py %d %d", repetitions, sleep_usecs);

    // Execute the Python script
    int status = system(command);

    if (status == 0)
    {
        // printf("Python script executed successfully.\n");

        // Read Poisson numbers from the file
        FILE *file = fopen("poisson_numbers.txt", "r");
        if (file == NULL)
        {
            fprintf(stderr, "Error opening file poisson_numbers.txt\n");
            exit(EXIT_FAILURE);
        }

        // Count the number of lines in the file
        int count = 0;
        int c;
        while ((c = fgetc(file)) != EOF)
        {
            if (c == '\n')
            {
                count++;
            }
        }

        // Allocate memory for the array
        int *poisson_numbers = (int *)malloc(count * sizeof(int));
        if (poisson_numbers == NULL)
        {
            fprintf(stderr, "Error allocating memory\n");
            exit(EXIT_FAILURE);
        }

        // Rewind the file and read numbers into the array
        rewind(file);
        for (int i = 0; i < count; ++i)
        {
            fscanf(file, "%d", &poisson_numbers[i]);
        }

        // Close the file
        fclose(file);

        return poisson_numbers;
    }
    else
    {
        fprintf(stderr, "Error executing Python script.\n");
        exit(EXIT_FAILURE);
    }
}

// // Function to generate Poisson-distributed random numbers
// int poissonRandomNumber(double lambda)
// {
//     double initial_lambda = lambda;
//     lambda = initial_lambda / (initial_lambda / 100);
//     double L = exp(-lambda);
//     double p = 1.0;
//     int k = 0;

//     do
//     {
//         k++;
//         p *= ((double)rand() / RAND_MAX);
//     } while (p > L);

//     return k * (lambda * 100);
// }

// double poisson_distribution(int k, double average)
// {
//     // Calculate e^(-lambda) * lambda^k / k!
//     return exp(-average) * pow(average, k) / tgamma(k + 1);
// }

// void generate_poisson_numbers(int k, double average, int *result)
// {
//     for (int i = 0; i < k; ++i)
//     {
//         double L = exp(-average);
//         double p = 1.0;
//         int x = -1;

//         do
//         {
//             ++x;
//             p *= ((double)rand() / (RAND_MAX));
//         } while (p > L);

//         result[i] = x;
//     }
// }

// void generate_poisson_numbers(int k, double average, unsigned int seed, int *result)
// {
//     gsl_rng *rng = gsl_rng_alloc(gsl_rng_default);
//     gsl_rng_set(rng, seed);

//     for (int i = 0; i < k; ++i)
//     {
//         result[i] = gsl_ran_poisson(rng, average);
//     }

//     gsl_rng_free(rng);
// }

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
        int valread = read(client_fd, buffer, 30);
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
    bool poisson, fixed, exponential, pre_request;
    char *hello = "Hello from client";
    char buffer[30] = {0};

    if (argc != 2)
    {
        printf("Usage: %s <parameter_file>\n", argv[0]);
        return -1;
    }

    char *serv_addr_ip;
    int sleep_usecs;
    int repetitions;
    int pre_request_interval = 0;

    // Read parameters from the file using the function
    readParameters(argv[1], &serv_addr_ip, &sleep_usecs, &repetitions, &poisson, &exponential, &fixed, &pre_request, &pre_request_interval);

    printf("Parameters:\nServer IP address: %s\nSleep microseconds: %d\nRepetitions: %d\nSend Pre request: %d\nPre-Request interval: %d\n", serv_addr_ip, sleep_usecs, repetitions, pre_request, pre_request_interval);
    char *distribution = (fixed) ? "Fixed" : (poisson) ? "Poisson"
                                                       : "Exponential";

    printf("Distribution: %s\n\n\n", distribution);

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

    int *poisson_numbers = get_poisson_numbers(repetitions, sleep_usecs);
    // if (poisson)
    // {
    //     // Allocate memory for the result array
    //     poisson_numbers = (int *)malloc(repetitions * sizeof(int));

    //     // Seed the random number generator
    //     unsigned int seed = (unsigned int)time(NULL);

    //     // Generate Poisson numbers
    //     generate_poisson_numbers(repetitions, sleep_usecs, seed, poisson_numbers);
    // }

    bool one_request = false;

    if (one_request)
    {
        send_request(true, client_fd1, hello, buffer, 8080);
        send_request(true, client_fd1, hello, buffer, 8080);
        usleep(10000);
        time_taken = send_request(true, client_fd1, hello, buffer, 8080);
        printf("Time taken for one request: %ld\n", time_taken);
        return -1;
    }

    // Seed the random number generator for the Poisson distribution
    srand((unsigned int)time(NULL));

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
        usleep(pre_request_interval); // Sleep for <pre_request_interval> us

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
            // int sleep_duration = poissonRandomNumber(sleep_usecs);
            // int sleep_duration = poisson_distribution(repetitions, sleep_usecs);
            fprintf(stderr, "Time sleep: %d\n", poisson_numbers[i]);
            usleep(poisson_numbers[i]);
        }
        else if (exponential)
        {
            int sleep_time = generateExponential(sleep_usecs);
            fprintf(stderr, "Time sleep: %d\n", sleep_time);
            usleep(sleep_time);
        }
        else if (fixed)
        {
            fprintf(stderr, "Time sleep: %d\n", sleep_usecs);
            usleep(sleep_usecs);
        }
        else
        {
            fprintf(stderr, "FATAL. No distribution found\nExiting ...\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Total time taken: %ld\n", total_time);
    // Close the connections (not reached in the current code)

    print_statistics(repetitions, latency_array);
    close(client_fd1);
    close(client_fd2);
    free(serv_addr_ip);
    free(latency_array);
    // Free allocated memory
    free(poisson_numbers);
    return 0;
}
