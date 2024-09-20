// multithreaded_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 1208
#define MESSAGE "Hello from client"

// Structure to pass client details to the thread
struct client_args {
    char *server_ip;
    int client_id;
};

// Function to handle communication with the server
void *connect_to_server(void *arg) {
    struct client_args *client = (struct client_args *)arg;
    int sock = 0;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};
    
    // Create a TCP socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Client %d: Socket creation error\n", client->client_id);
        return NULL;
    }

    // Set up server address (IP/Port)
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 and store server IP
    if (inet_pton(AF_INET, client->server_ip, &server_addr.sin_addr) <= 0) {
        printf("Client %d: Invalid address / Address not supported\n", client->client_id);
        return NULL;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Client %d: Connection to server failed\n", client->client_id);
        return NULL;
    }

    printf("Client %d: Connected to server at %s:%d\n", client->client_id, client->server_ip, PORT);

    // Send message to server
    send(sock, MESSAGE, strlen(MESSAGE), 0);
    printf("Client %d: Message sent to server: %s\n", client->client_id, MESSAGE);

    // Receive server response
    int bytes_read = read(sock, buffer, 1024);
    if (bytes_read > 0) {
        printf("Client %d: Received from server: %s\n", client->client_id, buffer);
    }

    // Close socket
    close(sock);
    printf("Client %d: Connection closed\n", client->client_id);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <num_clients>\n", argv[0]);
        return -1;
    }

    char *server_ip = argv[1];
    int num_clients = atoi(argv[2]);

    if (num_clients <= 0) {
        printf("Number of clients must be greater than 0\n");
        return -1;
    }

    pthread_t client_threads[num_clients];
    struct client_args clients[num_clients];

    // Create multiple clients and connect to the server
    for (int i = 0; i < num_clients; i++) {
        clients[i].server_ip = server_ip;
        clients[i].client_id = i + 1;

        pthread_create(&client_threads[i], NULL, connect_to_server, &clients[i]);
    }

    // Wait for all clients to finish
    for (int i = 0; i < num_clients; i++) {
        pthread_join(client_threads[i], NULL);
    }

    printf("All clients finished communication.\n");
    return 0;
}
