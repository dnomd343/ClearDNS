#ifndef PROCESS_H_
#define PROCESS_H_

#include <stdint.h>

typedef struct {
    int pid; // process id
    char *name; // caption
    char **cmd; // process command
    char **env; // environment variable
    char *cwd; // working directory
} process;

//extern process **process_list;

void process_list_run();
void process_list_init();
void process_list_daemon();
void process_list_append(process *proc);
void process_add_arg(process *proc, const char *arg);
process* process_init(const char *caption, const char *bin);

#endif
