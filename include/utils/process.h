#ifndef PROCESS_H_
#define PROCESS_H_

typedef struct {
    char *name;
    char **cmd;
    char **env;
    char *cwd;
} process;

void process_add_arg(process *proc, const char *arg);
process* process_init(const char *caption, const char *bin);

#endif