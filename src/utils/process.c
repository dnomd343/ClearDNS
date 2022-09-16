#include <stdlib.h>
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
