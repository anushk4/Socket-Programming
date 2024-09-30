#include "cpu.h"

#define BUFFER_SIZE 1024

// Function to retrive the top 2 process on the CPU
void get_top_cpu_processes(struct process_info top_processes[2]) {
    DIR *proc_dir = opendir("/proc");
    struct dirent *entry;

    // Initialize structures for the top 2 CPU processes with default values
    struct process_info top1 = { "", -1, 0, 0, 0 };
    struct process_info top2 = { "", -1, 0, 0, 0 };


    while ((entry = readdir(proc_dir)) != NULL) {
        // Check if the entry is a number (which indicates a process ID)
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            char stat_path[BUFFER_SIZE];

            // Build the path to the stat file for the process
            snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);

            // Open the stat file for the process
            FILE *stat_file = fopen(stat_path, "r");
            if (stat_file == NULL) {
                continue;
            }

            struct process_info proc;

            // Read the process ID, name, user CPU time, and kernel CPU time from the stat file
            fscanf(stat_file, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
                   &proc.pid, proc.name, &proc.user_time, &proc.kernel_time);
            fclose(stat_file);

            // Calculating total CPU time that is user time + kernel time)
            proc.total_cpu_time = proc.user_time + proc.kernel_time;

            // Comparing with the current top two processes and finally setting the top 2 process to return as output
            if (proc.total_cpu_time > top1.total_cpu_time) {
                top2 = top1;
                top1 = proc;
            } else if (proc.total_cpu_time > top2.total_cpu_time) {
                top2 = proc;
            }
        }
    }

    closedir(proc_dir);

    // Return the top two processes
    top_processes[0] = top1;
    top_processes[1] = top2;
}
