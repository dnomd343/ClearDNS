#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include "common.h"

int exiting = 0;

pid_t crond_pid = 0;
pid_t adguard_pid = 0;
pid_t overture_pid = 0;
pid_t domestic_dnsproxy_pid = 0;
pid_t foreign_dnsproxy_pid = 0;

void server_exit();
void get_sub_exit();
void server_daemon();
pid_t process_exec(char **command);
void init_server(char *init_script, char *custom_script);

void server_exit() { // kill sub-process and exit
    int status, ret;
    exiting = 1;
    fprintf(stderr, "[ClearDNS] Get exit signal.\n");

    if (crond_pid != 0) {
        fprintf(stderr, "[ClearDNS] Kill crond. (pid = %d)\n", crond_pid);
        kill(crond_pid, SIGTERM);
    }
    if (adguard_pid != 0) {
        fprintf(stderr, "[ClearDNS] Kill AdGuardHome. (pid = %d)\n", adguard_pid);
        kill(adguard_pid, SIGTERM);
    }
    if (overture_pid != 0) {
        fprintf(stderr, "[ClearDNS] Kill overture. (pid = %d)\n", overture_pid);
        kill(overture_pid, SIGTERM);
    }
    if (domestic_dnsproxy_pid != 0) {
        fprintf(stderr, "[ClearDNS] Kill domestic dnsproxy. (pid = %d)\n", domestic_dnsproxy_pid);
        kill(domestic_dnsproxy_pid, SIGTERM);
    }
    if (foreign_dnsproxy_pid != 0) {
        fprintf(stderr, "[ClearDNS] Kill foreign dnsproxy. (pid = %d)\n", foreign_dnsproxy_pid);
        kill(foreign_dnsproxy_pid, SIGTERM);
    }

    while ((ret = wait(&status)) != -1) {
        fprintf(stderr, "[ClearDNS] Subprocess exit. (pid = %d)\n", ret);
    }
    printf("[ClearDNS] Exit successfully.\n");
    exit(0);
}

void get_sub_exit() { // catch child process die
    int ret, status;
    int wait_time = 3; // seconds for wait before restart
    if (exiting) { return; } // server daemon is exiting

    if (crond_pid != 0) { // check crond
        ret = waitpid(crond_pid, &status, WNOHANG); // non-blocking
        if (ret == -1) {
            perror("[ClearDNS] Waitpid error");
            server_exit();
        } else if (ret) { // process exit
            crond_pid = process_exec(crond_command);
            fprintf(stderr, "[ClearDNS] Catch crond exit and restart it. (pid = %d)\n", crond_pid);
            sleep(wait_time); // reduce restart frequency
            return;
        }
    }

    if (adguard_pid != 0) { // check AdGuardHome
        ret = waitpid(adguard_pid, &status, WNOHANG); // non-blocking
        if (ret == -1) {
            perror("[ClearDNS] Waitpid error");
            server_exit();
        } else if (ret) { // process exit
            adguard_pid = process_exec(adguard_command);
            fprintf(stderr, "[ClearDNS] Catch AdGuardHome exit and restart it. (pid = %d)\n", adguard_pid);
            sleep(wait_time); // reduce restart frequency
            return;
        }
    }

    if (overture_pid != 0) { // check overture
        ret = waitpid(overture_pid, &status, WNOHANG); // non-blocking
        if (ret == -1) {
            perror("[ClearDNS] Waitpid error");
            server_exit();
        } else if (ret) { // process exit
            overture_pid = process_exec(overture_command);
            fprintf(stderr, "[ClearDNS] Catch overture exit and restart it. (pid = %d)\n", overture_pid);
            sleep(wait_time); // reduce restart frequency
            return;
        }
    }

    if (domestic_dnsproxy_pid != 0) { // check domestic dnsproxy
        ret = waitpid(domestic_dnsproxy_pid, &status, WNOHANG); // non-blocking
        if (ret == -1) {
            perror("[ClearDNS] Waitpid error");
            server_exit();
        } else if (ret) { // process exit
            domestic_dnsproxy_pid = process_exec(domestic_dnsproxy_command);
            fprintf(stderr, "[ClearDNS] Catch domestic dnsproxy exit and restart it. (pid = %d)\n", domestic_dnsproxy_pid);
            sleep(wait_time); // reduce restart frequency
            return;
        }
    }

    if (foreign_dnsproxy_pid != 0) { // check foreign dnsproxy
        ret = waitpid(foreign_dnsproxy_pid, &status, WNOHANG); // non-blocking
        if (ret == -1) {
            perror("[ClearDNS] Waitpid error");
            server_exit();
        } else if (ret) { // process exit
            foreign_dnsproxy_pid = process_exec(foreign_dnsproxy_command);
            fprintf(stderr, "[ClearDNS] Catch foreign dnsproxy exit and restart it. (pid = %d)\n", foreign_dnsproxy_pid);
            sleep(wait_time); // reduce restart frequency
            return;
        }
    }

    ret = waitpid(-1, &status, WNOHANG); // waitpid for all sub-process (non-blocking)
    if (ret == -1) {
        perror("[ClearDNS] Waitpid error");
        server_exit();
    } else if (ret) { // process exit
        fprintf(stderr, "[ClearDNS] Catch subprocess exit. (pid = %d)\n", ret);
        return;
    }
    fprintf(stderr, "[ClearDNS] Subprocess not found.\n"); // get SIGCHLD signal but not exited subprocess found
}

pid_t process_exec(char **command) {
    pid_t pid;
    char **env = {NULL};
    if ((pid = fork()) < 0) {
        perror("[ClearDNS] Fork error");
        exit(2);
    } else if (pid == 0) { // child process
        prctl(PR_SET_PDEATHSIG, SIGKILL); // child process die with father process
        if (execvpe(command[0], command, env) < 0) {
            perror("[ClearDNS] Exec error");
            exit(3);
        }
    }
    return pid;
}

void init_server(char *init_script, char *custom_script) { // run init script (blocking) / custom script (non-blocking)
    int status;
    pid_t init_pid, custom_pid;

    if ((init_pid = fork()) < 0) {
        perror("[ClearDNS] Fork error");
        exit(2);
    } else if (init_pid == 0) { // child process
        prctl(PR_SET_PDEATHSIG, SIGKILL); // child process die with father process
        if (execlp("/bin/sh", "/bin/sh", init_script, NULL)) {
            perror("[ClearDNS] Init error");
            exit(3);
        }
    }
    wait(&status); // blocking wait
    fprintf(stderr, "[ClearDNS] Init complete.\n");

    if (access(custom_script, F_OK) >= 0) { // custom script exist
        fprintf(stderr, "[ClearDNS] Running custom script.\n");
        if ((custom_pid = fork()) < 0) {
            perror("[ClearDNS] Fork error");
            exit(2);
        } else if (custom_pid == 0) { // child process
            prctl(PR_SET_PDEATHSIG, SIGKILL); // child process die with father process
            if (execlp("/bin/sh", "/bin/sh", custom_script, NULL)) {
                perror("[ClearDNS] Custom script error");
                exit(3);
            }
        }
        sleep(1); // wait a moment
    }
}

void server_daemon() { // run as a daemon of cleardns
    int wait_us_time = 200 * 1000; // 200ms
    signal(SIGINT, server_exit); // catch Ctrl + C (2)
    signal(SIGTERM, server_exit); // catch exit signal (15)
    signal(SIGCHLD, get_sub_exit); // callback when child process die

    crond_pid = process_exec(crond_command);
    fprintf(stderr, "[ClearDNS] Exec crond. (pid = %d)\n", crond_pid);
    usleep(wait_us_time);

    adguard_pid = process_exec(adguard_command);
    fprintf(stderr, "[ClearDNS] Exec AdGuardHome. (pid = %d)\n", adguard_pid);
    usleep(wait_us_time);

    overture_pid = process_exec(overture_command);
    fprintf(stderr, "[ClearDNS] Exec overture. (pid = %d)\n", overture_pid);
    usleep(wait_us_time);

    domestic_dnsproxy_pid = process_exec(domestic_dnsproxy_command);
    fprintf(stderr, "[ClearDNS] Exec domestic dnsproxy. (pid = %d)\n", domestic_dnsproxy_pid);
    usleep(wait_us_time);

    foreign_dnsproxy_pid = process_exec(foreign_dnsproxy_command);
    fprintf(stderr, "[ClearDNS] Exec foreign dnsproxy. (pid = %d)\n", foreign_dnsproxy_pid);
    usleep(wait_us_time);

    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);
}
