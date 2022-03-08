#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>

void process_exec() {

}

void init_server(char *init_script, char *custom_script) { // run init script (blocking) / custom script (non-blocking)
    int status;
    pid_t init_pid, custom_pid;

    if ((init_pid = fork()) < 0) {
        perror("Fork error");
        exit(2);
    } else if (init_pid == 0) { // child process
        prctl(PR_SET_PDEATHSIG, SIGKILL); // child process die with father process
        if (execlp("/bin/sh", "/bin/sh", init_script, NULL)) {
            perror("Init error");
            exit(3);
        }
    }
    wait(&status); // blocking wait
    fprintf(stderr, "Init complete\n");

    if (access(custom_script, F_OK) >= 0) { // custom script exist
        if ((custom_pid = fork()) < 0) {
            perror("Fork error");
            exit(2);
        } else if (custom_pid == 0) { // child process
            prctl(PR_SET_PDEATHSIG, SIGKILL); // child process die with father process
            if (execlp("/bin/sh", "/bin/sh", custom_script, NULL)) {
                perror("Custom script error");
                exit(3);
            }
        }
    }
}

void server_daemon() {

}
