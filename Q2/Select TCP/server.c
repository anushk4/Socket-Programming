#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

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

void *handle_client(int *client_sockets, int i){
    int client_sock = client_sockets[i];
    // Convert the client's IP address to human-readable form
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
        // Handle the client's request and send the response
        buffer[bytes_read] = '\0';
        struct process_info top_processes[2];
        get_top_cpu_processes(top_processes);

        printf("Top 2 CPU-consuming processes sent to client %s:%d:\n", client_ip, client_port);
        printf("1. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n",
            top_processes[0].pid, top_processes[0].name, top_processes[0].user_time, top_processes[0].kernel_time);
        printf("2. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n",
            top_processes[1].pid, top_processes[1].name, top_processes[1].user_time, top_processes[1].kernel_time);

        snprintf(buffer, sizeof(buffer),
                    "Top 2 CPU-consuming processes:\n"
                    "1. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n"
                    "2. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n",
                    top_processes[0].pid, top_processes[0].name, top_processes[0].user_time, top_processes[0].kernel_time,
                    top_processes[1].pid, top_processes[1].name, top_processes[1].user_time, top_processes[1].kernel_time);

        send(client_sock, buffer, strlen(buffer), 0);
    }
    // Client has disconnected
    printf("Client disconnected: socket FD is %d\n", client_sock);
    close(client_sock);
    client_sockets[i] = 0; // Remove from client sockets list
}

int main() {
    int server_sock, client_sock, max_sd, activity;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    fd_set read_fds; // Set of socket descriptors

    // Store client sockets
    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0; // Initialize all client sockets to 0
    }

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

    // Main server loop
    while (1) {
        // Clear the socket set
        FD_ZERO(&read_fds);

        // Add the server socket to the set
        FD_SET(server_sock, &read_fds);
        max_sd = server_sock;

        // Add child sockets to the set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            client_sock = client_sockets[i];

            // If the client socket is a valid descriptor, add it to the set
            if (client_sock > 0) {
                FD_SET(client_sock, &read_fds);
            }

            // Keep track of the maximum socket descriptor
            if (client_sock > max_sd) {
                max_sd = client_sock;
            }
        }

        // Use select to monitor the sockets for activity
        activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            perror("Select error");
        }

        // Check if there is a new incoming connection on the server socket
        if (FD_ISSET(server_sock, &read_fds)) {
            int new_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
            if (new_sock < 0) {
                perror("Accept failed");
                continue;
            }

            printf("New connection: socket FD is %d, IP is : %s, PORT : %d\n",
                   new_sock, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Add the new socket to the list of client sockets
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_sock;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        // Check all client sockets for incoming data
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(client_sockets[i], &read_fds)) {
                handle_client(client_sockets, i);
            }
        }
    }

    // Close the server socket
    close(server_sock);
    return 0;
}
