#include "handleClient.c"

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

        connect_to_server(&clients[i]);
    }

    printf("All clients finished communication.\n");
    return 0;
}
