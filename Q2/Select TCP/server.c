#include "handleClient.c"

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

    // Initiate the server with proper IP and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
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
                handle_client_for_connection(client_sockets, i);
            }
        }
    }

    // Close the server socket
    close(server_sock);
    return 0;
}
