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

uint8_t EXITING = FALSE;
uint8_t EXITED = FALSE;

typedef struct {
    int pid;
    int exit_code;
    int exit_signal;
} exit_info;

void get_sub_exit();
void get_exit_signal();
void server_exit(int exit_code);
void process_dump(process *proc);
void process_exec(process *proc);
exit_info get_exit_info(int status, pid_t pid);
void show_exit_info(exit_info info, char *prefix);

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
    log_debug("%s env variable -> %s", proc->name, process_env);
    log_debug("%s cwd -> %s", proc->name, proc->cwd);
    log_debug("%s command -> %s", proc->name, process_cmd);
    free(process_env);
    free(process_cmd);
}

void process_exec(process *proc) {
    pid_t pid;
    log_info("%s start", proc->name);
    process_dump(proc);
    if ((pid = fork()) < 0) { // fork error
        log_perror("%s fork error", proc->name);
        server_exit(EXIT_FORK_ERROR);
    } else if (pid == 0) { // child process
        if (chdir(proc->cwd)) { // change working directory
            log_perror("%s with invalid cwd `%s` -> ", proc->name, proc->cwd);
            exit(EXIT_EXEC_ERROR);
        }
        prctl(PR_SET_PDEATHSIG, SIGKILL); // child process die with father process
        if (execvpe(*(proc->cmd), proc->cmd, proc->env) < 0) {
            log_perror("%s exec error -> ", proc->name);
            exit(EXIT_EXEC_ERROR);
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
    signal(SIGQUIT, get_exit_signal); // catch Ctrl + \ (3)
    signal(SIGTERM, get_exit_signal); // catch exit signal (15)
    signal(SIGCHLD, get_sub_exit); // callback when child process die
    for (process **proc = process_list; *proc != NULL; ++proc) {
        process_exec(*proc);
        usleep(200 * 1000); // delay 200ms
    }
    log_info("Process start complete");
}

void process_list_daemon() {
    while (!EXITED) {
        pause();
    }
}

void show_exit_info(exit_info info, char *prefix) { // show info of child process death
    if (info.exit_code != -1) { // exit normally
        log_warn("%s (PID = %d) -> Exit code %d", prefix, info.pid, info.exit_code);
    } else if (info.exit_signal != -1) { // abnormal exit
        log_warn("%s (PID = %d) -> Killed by signal %d", prefix, info.pid, info.exit_signal);
    } else {
        log_warn("%s (PID = %d) -> Unknown reason", prefix, info.pid);
    }
}

exit_info get_exit_info(int status, pid_t pid) { // get why the child process death
    exit_info temp;
    temp.pid = pid;
    temp.exit_code = temp.exit_signal = -1;
    if (WIFEXITED(status)) { // exit normally (with an exit-code)
        temp.exit_code = WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) { // abnormal exit (with a signal)
        temp.exit_signal = WTERMSIG(status);
    }
    return temp;
}

void server_exit(int exit_code) { // kill sub process and exit
    while (EXITING) { // only run once
        pause();
    }
    EXITING = TRUE;
    log_info("ClearDNS start exit process");
    for (process **proc = process_list; *proc != NULL; ++proc) { // send SIGTERM to all subprocess
        log_info("%s send kill signal -> PID = %d", (*proc)->name, (*proc)->pid);
        kill((*proc)->pid, SIGTERM);
    }
    for (process **proc = process_list; *proc != NULL; ++proc) { // ensure all subprocess exited
        int status;
        log_debug("%s start blocking wait", (*proc)->name);
        int ret = waitpid((*proc)->pid, &status, 0); // blocking wait
        log_info("%s exit -> PID = %d", (*proc)->name, ret);
    }
    EXITED = TRUE;
    log_warn("ClearDNS exit");
    exit(exit_code);
}

void get_exit_signal() { // get SIGINT or SIGTERM signal
    log_info("Get exit signal");
    server_exit(EXIT_NORMAL); // normally exit
}

void get_sub_exit() { // catch child process exit
    if (EXITING) {
        log_debug("Skip SIGCHLD signal");
        return;
    }
    int status;
    log_debug("Handle sub-process exit");
    for (process **proc = process_list; *proc != NULL; ++proc) {
        if ((*proc)->pid == 0) {
            continue; // skip not running process
        }
        int wait_ret = waitpid((*proc)->pid, &status, WNOHANG); // non-blocking wait
        if (wait_ret == -1) { // process wait error
            log_perror("%s waitpid error -> ", (*proc)->name);
            server_exit(EXIT_WAIT_ERROR);
        } else if (wait_ret) { // catch process exit
            show_exit_info(get_exit_info(status, (*proc)->pid), (*proc)->name);
            sleep(3); // reduce restart frequency
            process_exec(*proc);
            usleep(200 * 1000); // delay 200ms
            log_info("%s restart complete", (*proc)->name);
            return; // skip following check
        }
    }
    int wait_ret = waitpid(-1, &status, WNOHANG); // waitpid for all sub-process (non-blocking)
    if (wait_ret == -1) {
        log_perror("Waitpid error");
        server_exit(EXIT_WAIT_ERROR);
    } else if (wait_ret) { // process exit
        show_exit_info(get_exit_info(status, wait_ret), "Sub-process");
    }
    log_debug("Handle sub-process complete");
}
