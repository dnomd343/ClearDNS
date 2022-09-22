#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include "logger.h"
#include "sundry.h"
#include "process.h"
#include "constant.h"
#include "structure.h"

process **process_list;

void get_sub_exit();
void get_exit_signal();
void server_exit(int exit_code);
void process_dump(process *proc);
void process_exec(process *proc);

process* process_init(const char *caption, const char *bin) { // init process struct
    process *proc = (process *)malloc(sizeof(process));
    proc->pid = 0; // process not running
    proc->name = string_init(caption); // process caption
    proc->cmd = string_list_init();
    string_list_append(&proc->cmd, bin); // argv[0] normally be process file name
    proc->env = string_list_init(); // empty environment variable
    proc->cwd = WORK_DIR; // current working directory
    return proc;
}

void process_dump(process *proc) { // output process options into debug log
    char *process_cmd = string_list_dump(proc->cmd);
    char *process_env = string_list_dump(proc->env);
    log_debug("%s cwd -> %s", proc->name, proc->cwd);
    log_debug("%s command -> %s", proc->name, process_cmd);
    log_debug("%s env variable -> %s", proc->name, process_env);
    free(process_env);
    free(process_cmd);
}

void process_exec(process *proc) {
    pid_t pid;
    process_dump(proc);
    log_info("%s start", proc->name);

    if ((pid = fork()) < 0) { // fork error
        log_perror("%s fork error", proc->name);
        server_exit(EXIT_FORK_ERROR);
    } else if (pid == 0) { // child process
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
    proc->pid = pid;
    log_info("%s running success -> PID = %d", proc->name, proc->pid);
}

void process_add_arg(process *proc, const char *arg) { // add argument for process
    string_list_append(&proc->cmd, arg);
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

void process_list_run() { // start process list
    signal(SIGINT, get_exit_signal); // catch Ctrl + C (2)
    signal(SIGTERM, get_exit_signal); // catch exit signal (15)
    signal(SIGCHLD, get_sub_exit); // callback when child process die
    for (process **p = process_list; *p != NULL; ++p) {
        process_exec(*p);
        usleep(100 * 1000); // delay 100ms
    }
    log_info("Process start complete");
}

void process_list_daemon() {
    int status;
    wait(&status);
}

void server_exit(int exit_code) { // kill sub process and exit
    log_info("Kill subprocess");

    // TODO: kill subprocess and exit cleardns

    exit(exit_code);
}

void get_exit_signal() { // get SIGINT or SIGTERM signal
    log_info("Get exit signal");
    server_exit(EXIT_NORMAL);
}

void get_sub_exit() { // catch child process exit
    log_debug("Get SIGCHLD signal");

    // TODO: check exit subprocess and restart it
}
