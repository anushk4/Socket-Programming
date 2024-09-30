#include "handleClient.c"

int main(int argc, char *argv[]) {
    // Check to ensure the necessary 3 arguments are passed through command line
    if (argc != 3) {
        printf("Usage: %s <server_ip> <num_clients>\n", argv[0]);
        return -1;
    }

    char *server_ip = argv[1];
    int num_clients = atoi(argv[2]);

    // Checking the number of client should be greater than 0
    if (num_clients <= 0) {
        printf("Number of clients must be greater than 0\n");
        return -1;
    }

    // To make multiple threads, each for a client
    pthread_t client_threads[num_clients];
    struct client_args clients[num_clients];

    // Initiate multiple clients to connect to server
    for (int i = 0; i < num_clients; i++) {
        clients[i].server_ip = server_ip;
        clients[i].client_id = i + 1;

        pthread_create(&client_threads[i], NULL, connect_to_server, &clients[i]); // To ensure multithreading
    }

    // Waiting for all clients to finish the execution and then return the result using join
    for (int i = 0; i < num_clients; i++) {
        pthread_join(client_threads[i], NULL);
    }

    printf("All clients finished communication.\n");
    return 0;
}
