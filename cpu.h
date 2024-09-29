#ifndef CALL_CPU_H
#define CALL_CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "structures.h"

#define BUFFER_SIZE 1024
#define MAX_NAME_SIZE 256

void get_top_cpu_processes(struct process_info top_processes[2]);

#endif 