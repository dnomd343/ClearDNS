#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <stdio.h>
#include "sundry.h"
#include "process.h"
#include "constant.h"
#include "structure.h"
#include "logger.h"

process **process_list;

process* process_init(const char *caption, const char *bin) { // init process struct
    process *proc = (process *)malloc(sizeof(process));
    proc->name = string_init(caption); // process caption
    proc->cmd = string_list_append(string_list_init(), bin); // argv[0] normally be process file name
    proc->env = string_list_init(); // empty environment variable
    proc->cwd = WORK_DIR; // current working directory
    proc->is_group = FALSE; // create new process group
    return proc;
}

void process_add_arg(process *proc, const char *arg) { // add argument for process
    proc->cmd = string_list_append(proc->cmd, arg);
}

void process_list_init() { // init process list
    process_list = (process **)malloc(sizeof(process *));
    *process_list = NULL;
}

void process_list_append(process *proc) { // add new process into process list
    int proc_num = 0;
    for (process **p = process_list; *p != NULL; ++p, ++proc_num); // get process number
    process_list = (process **)realloc(process_list, sizeof(process *) * (proc_num + 2));
    process_list[proc_num++] = proc; // add into process list
    process_list[proc_num] = NULL;
    log_debug("%s process added", proc->name);
}

void process_list_run() {
    for (process **p = process_list; *p != NULL; ++p) {
        log_warn("proc -> %s", (*p)->name);
    }
}

void server_exit() {
    // TODO: kill subprocess and exit cleardns
}


void process_dump(process *proc) {
    char *process_cmd = string_list_dump(proc->cmd);
    char *process_env = string_list_dump(proc->env);
    log_debug("%s cwd -> %s", proc->name, proc->cwd);
    log_debug("%s command -> %s", proc->name, process_cmd);
    log_debug("%s env variable -> %s", proc->name, process_env);
    log_debug("%s new group -> %s", proc->name, show_bool(proc->is_group));
    free(process_env);
    free(process_cmd);
}

pid_t process_exec(process *proc) {
    pid_t pid;

    process_dump(proc);

    if ((pid = fork()) < 0) { // fork error
        log_perror("Fork error");
        server_exit();
    } else if (pid == 0) { // child process
        // TODO: new process group

        if (proc->is_group) {
            setpgrp(); // new process group
        }

        if (chdir(proc->cwd)) { // change working directory
            log_perror("%s with invalid cwd `%s` -> ", proc->name, proc->cwd);
            exit(2);
        }

        prctl(PR_SET_PDEATHSIG, SIGKILL); // child process die with father process
        if (execvpe(*(proc->cmd), proc->cmd, proc->env) < 0) {
            log_perror("%s exec error -> ", proc->name);
            exit(1);
        }
    }

    return pid;

}

