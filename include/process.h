#ifndef _PROCESS_H_
#define _PROCESS_H_

//void server_daemon();
//void init_server(char *init_script, char *custom_script);

typedef struct {
    char **cmd;
    char **env;
    char *cwd;
} process;

#endif
