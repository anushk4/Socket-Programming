// multithreaded_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 1208
#define MAX_CLIENTS 10

// Structure to hold client socket information
struct client_info {
    int socket;
    struct sockaddr_in client_addr;
};

// Function to handle communication with the client
void *handle_client(void *arg) {
    struct client_info *client = (struct client_info*)arg;
    char buffer[1024] = {0};
    char *message = "Hello from server";
    
    // Get client details (IP, Port)
    char *client_ip = inet_ntoa(client->client_addr.sin_addr);
    int client_port = ntohs(client->client_addr.sin_port);
    
    printf("Client connected: IP = %s, Port = %d\n", client_ip, client_port);

    // Read client message
    int bytes_read = read(client->socket, buffer, 1024);
    if (bytes_read > 0) {
        printf("Received from client [%s:%d]: %s\n", client_ip, client_port, buffer);
        
        // Send response back to the client
        send(client->socket, message, strlen(message), 0);
        printf("Message sent to client [%s:%d]: %s\n", client_ip, client_port, message);
    }

    // Close client socket
    close(client->socket);
    free(client); // Free dynamically allocated memory for client_info
    printf("Connection closed with client [%s:%d].\n", client_ip, client_port);
    
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

        // Allocate memory to store client information
        struct client_info *client = malloc(sizeof(struct client_info));
        client->socket = new_sock;
        client->client_addr = client_addr;

        // Create a new thread to handle the client connection
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, client);
        pthread_detach(client_thread); // Detach thread so resources are freed when it finishes
    }

    // Close server socket (this point is never reached in this implementation)
    close(server_sock);
    return 0;
}
