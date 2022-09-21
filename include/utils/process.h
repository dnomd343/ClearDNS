#ifndef PROCESS_H_
#define PROCESS_H_

#include <stdint.h>

typedef struct {
    char *name;
    char **cmd;
    char **env;
    char *cwd;
    int8_t is_group; // bool value
} process;

void process_list_run();
void process_list_init();
void process_list_append(process *proc);
void process_add_arg(process *proc, const char *arg);
process* process_init(const char *caption, const char *bin);

#include <unistd.h>
pid_t process_exec(process *proc);

#endif
