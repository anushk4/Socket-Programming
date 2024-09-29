#ifndef STRUCTURE_H
#define STRUCTURE_H

struct process_info {
    char name[256];
    int pid;
    long unsigned int user_time;
    long unsigned int kernel_time;
    long unsigned int total_cpu_time; // user_time + kernel_time
};

struct client_info {
    int socket;
    struct sockaddr_in client_addr;
};

// Structure to pass client details to the thread
struct client_args {
    char *server_ip;
    int client_id;
};

#endif 