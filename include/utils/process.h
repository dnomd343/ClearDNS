#ifndef PROCESS_H_
#define PROCESS_H_

#include <stdint.h>

typedef struct {
    int pid;
    char *name;
    char **cmd;
    char **env;
    char *cwd;
} process;

void process_list_run();
void process_list_init();
void process_list_append(process *proc);
void process_add_arg(process *proc, const char *arg);
process* process_init(const char *caption, const char *bin);

#include <unistd.h>
void process_exec(process *proc);
void process_list_daemon();

#endif
