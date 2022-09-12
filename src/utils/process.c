#include "common.h"
#include "process.h"
#include "structure.h"

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
