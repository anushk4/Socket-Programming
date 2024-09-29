#ifndef CONNECTING_SERVER_H
#define CONNECTING_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "structures.h"

#define PORT 8080
#define MESSAGE "Hello from client"
#define MAX_CLIENTS 10

void *connect_to_server(void *arg);

#endif