// multithreaded_server_topcpu.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 1208
#define MAX_CLIENTS 7
#define BUFFER_SIZE 1024

struct process_info {
    char name[BUFFER_SIZE];
    int pid;
    long unsigned int user_time;
    long unsigned int kernel_time;
    long unsigned int total_cpu_time; // user_time + kernel_time
};

// Function to get the top 2 CPU-consuming processes
void get_top_cpu_processes(struct process_info top_processes[2]) {
    DIR *proc_dir = opendir("/proc");
    struct dirent *entry;

    struct process_info top1 = { "", -1, 0, 0, 0 };
    struct process_info top2 = { "", -1, 0, 0, 0 };

    while ((entry = readdir(proc_dir)) != NULL) {
        // Check if the entry is a number (which indicates a process ID)
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            char stat_path[BUFFER_SIZE];
            snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);

            FILE *stat_file = fopen(stat_path, "r");
            if (stat_file == NULL) {
                continue;
            }

            struct process_info proc;
            fscanf(stat_file, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
                   &proc.pid, proc.name, &proc.user_time, &proc.kernel_time);
            fclose(stat_file);

            // Calculate total CPU time (user time + kernel time)
            proc.total_cpu_time = proc.user_time + proc.kernel_time;

            // Compare with the current top two processes
            if (proc.total_cpu_time > top1.total_cpu_time) {
                top2 = top1;
                top1 = proc;
            } else if (proc.total_cpu_time > top2.total_cpu_time) {
                top2 = proc;
            }
        }
    }

    closedir(proc_dir);

    // Return the top two processes
    top_processes[0] = top1;
    top_processes[1] = top2;
}

// Function to handle client connection
void *handle_client(int client_sock) {
    char buffer[BUFFER_SIZE] = {0};
    struct process_info top_processes[2];

    // Get the client's address
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_sock, (struct sockaddr *)&client_addr, &addr_len);

    // Convert the client's IP address to human-readable form
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr.sin_port);

    printf("Connected to client at %s:%d\n", client_ip, client_port);

    // Read client request (could be any message, we assume a simple message here)
    int bytes_read = read(client_sock, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
        printf("Received request from client: %s\n", buffer);

        // Get the top 2 CPU-consuming processes
        get_top_cpu_processes(top_processes);

        // Log the top 2 processes to the server console
        printf("Top 2 CPU-consuming processes sent to client %s:%d:\n", client_ip, client_port);
        printf("1. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n",
               top_processes[0].pid, top_processes[0].name, top_processes[0].user_time, top_processes[0].kernel_time);
        printf("2. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n",
               top_processes[1].pid, top_processes[1].name, top_processes[1].user_time, top_processes[1].kernel_time);

        // Prepare the response with the top 2 processes' information
        snprintf(buffer, sizeof(buffer),
                 "Top 2 CPU-consuming processes:\n"
                 "1. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n"
                 "2. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n",
                 top_processes[0].pid, top_processes[0].name, top_processes[0].user_time, top_processes[0].kernel_time,
                 top_processes[1].pid, top_processes[1].name, top_processes[1].user_time, top_processes[1].kernel_time);

        // Send the response back to the client
        send(client_sock, buffer, strlen(buffer), 0);
    }

    // Close the client socket
    close(client_sock);
    return NULL;
}

int main() {
    int server_sock, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create a TCP socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address (IP/Port)
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the IP/port
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Main server loop: Accept and handle incoming client connections
    while (1) {
        new_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (new_sock < 0) {
            perror("Accept failed");
            continue;
        }
        // printf("Going to handle client\n");
        handle_client(new_sock);
    }

    // Close server socket (this point is never reached in this implementation)
    close(server_sock);
    return 0;
}
