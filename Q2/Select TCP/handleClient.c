#include "handleClient.h"

void *handle_client_for_connection(int *client_sockets, int i){
    int client_sock = client_sockets[i];
    
    char buffer[BUFFER_SIZE] = {0};
    struct process_info top_processes[2];

     // Retrieve the client addresss to let the server know which client is sending the connection request
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_sock, (struct sockaddr *)&client_addr, &addr_len);

    // Convert the client's IP address to human-readable form
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr.sin_port);

    printf("Connected to client at %s:%d\n", client_ip, client_port);

    // Read and check the client requesst, here we have hardcoded the message to check that server has recieved a request
    int bytes_read = read(client_sock, buffer, BUFFER_SIZE);

    if (bytes_read > 0) {
        // Handle the client's request and send the response
        buffer[bytes_read] = '\0';
        struct process_info top_processes[2];

        // Calling the function to retrieve the 2 top 2 processes running on server side
        get_top_cpu_processes(top_processes);

        // Print the process info on server side
        printf("Top 2 CPU-consuming processes sent to client %s:%d:\n", client_ip, client_port);
        printf("1. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n",
            top_processes[0].pid, top_processes[0].name, top_processes[0].user_time, top_processes[0].kernel_time);
        printf("2. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n",
            top_processes[1].pid, top_processes[1].name, top_processes[1].user_time, top_processes[1].kernel_time);

        // Make the response to be send to the client that is the process info
        snprintf(buffer, sizeof(buffer),
                    "Top 2 CPU-consuming processes:\n"
                    "1. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n"
                    "2. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu",
                    top_processes[0].pid, top_processes[0].name, top_processes[0].user_time, top_processes[0].kernel_time,
                    top_processes[1].pid, top_processes[1].name, top_processes[1].user_time, top_processes[1].kernel_time);

        send(client_sock, buffer, strlen(buffer), 0);
    }
    // Close the client socket after all the requests are taken care of
    printf("Client disconnected: socket FD is %d\n", client_sock);
    close(client_sock);
    client_sockets[i] = 0; // Remove from client sockets list
}
