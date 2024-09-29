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
#include "../../cpu.c"
#include "../../connectToServer.c"

void *handle_client_for_connection(int client_sock);

#endif // HANDLE_CLIENT_H