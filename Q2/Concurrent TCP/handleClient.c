#include "handleClient.h"

void *handle_client_for_connection(void *arg) {
    int client_sock = *(int *)arg;
    free(arg); // Free dynamically allocated memory for client socket

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
                 "2. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu",
                 top_processes[0].pid, top_processes[0].name, top_processes[0].user_time, top_processes[0].kernel_time,
                 top_processes[1].pid, top_processes[1].name, top_processes[1].user_time, top_processes[1].kernel_time);

        // Send the response back to the client
        send(client_sock, buffer, strlen(buffer), 0);
    }

    // Close the client socket
    close(client_sock);
    return NULL;
}