#ifndef _PROCESS_H_
#define _PROCESS_H_

//void server_daemon();
//void init_server(char *init_script, char *custom_script);

typedef struct {
    char *name;
    char **cmd;
    char **env;
    char *cwd;
} process;

void process_add_arg(process *proc, const char *arg);
process* process_init(const char *caption, const char *bin);

#endif
