#ifndef HANDLE_CLIENT_H
#define HANDLE_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "../cpu.c"
#include "../connectToServer.c"

// Declare the functions that are to be used in handleClient.c
void *handle_client_for_getting_CPU_Process(void *arg);
void *handle_client_for_connection(void *arg);

#endif // HANDLE_CLIENT_H
