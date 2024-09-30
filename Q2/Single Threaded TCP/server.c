#include "handleClient.c"

int main() {
    int server_sock, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Creating a TCP socket as described in the tutorial
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Set socket options failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Set up the server address (IP/Port)
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Initiate the server with proper IP and port
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

    // Loop for accept and handle incoming client connections that is to etablish a connection.
    while (1) {
        new_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (new_sock < 0) {
            perror("Accept failed");
            continue;
        }
        handle_client_for_connection(new_sock);
    }

    // Close server socket (this point is never reached in this implementation) as we are taking assumption that server will keep listening to client requests at the same port untill called exit manually.
    close(server_sock);
    return 0;
}
