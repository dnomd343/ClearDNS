#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <glib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include "constant.h"
#include "flag_legacy.h"

int exiting = 0;

pid_t custom_pgid = 0;
pid_t adguard_pid = 0;
pid_t overture_pid = 0;
pid_t domestic_dnsproxy_pid = 0;
pid_t foreign_dnsproxy_pid = 0;

void get_sub_exit();
void server_daemon();
void get_exit_signal();
void server_exit(int exit_code);
pid_t process_exec(char **command, int is_new_group);
void init_server(char *init_script, char *custom_script);

void get_exit_signal() { // get SIGINT or SIGTERM signal
    fprintf(stderr, "[ClearDNS] Get exit signal.\n");
    server_exit(EXIT_NORMAL);
}

void server_exit(int exit_code) { // kill sub process and exit
    int status, ret;
    exiting = 1; // set a exit flag

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

    ret = waitpid(adguard_pid, &status, 0); // blocking
    fprintf(stderr, "[ClearDNS] AdGuardHome exit. (pid = %d)\n", ret);

    ret = waitpid(overture_pid, &status, 0); // blocking
    fprintf(stderr, "[ClearDNS] overture exit. (pid = %d)\n", ret);

    ret = waitpid(domestic_dnsproxy_pid, &status, 0); // blocking
    fprintf(stderr, "[ClearDNS] domestic dnsproxy exit. (pid = %d)\n", ret);

    ret = waitpid(foreign_dnsproxy_pid, &status, 0); // blocking
    fprintf(stderr, "[ClearDNS] foreign dnsproxy exit. (pid = %d)\n", ret);

    if (custom_pgid != 0) {
        ret = waitpid(-custom_pgid, &status, WNOHANG);
        if (ret == 0) { // custom script still running
            fprintf(stderr, "[ClearDNS] Kill custom script. (pgid = %d)\n", custom_pgid);
            kill(-custom_pgid, SIGTERM); // kill process group
        }
    }

    fprintf(stderr, "[ClearDNS] Kill all subprocess.\n");
    kill(-1, SIGTERM); // send kill signal to all process
    while ((ret = wait(&status)) != -1) {
        fprintf(stderr, "[ClearDNS] Subprocess exit. (pid = %d)\n", ret);
    }

    fprintf(stderr, "[ClearDNS] Exit successfully.\n");
    exit(exit_code);
}

void get_sub_exit() { // catch child process exit
    int ret, status;
    int wait_time = 3; // seconds for wait before restart
    if (exiting) { return; } // server daemon is exiting

    if (adguard_pid != 0) { // check AdGuardHome
        ret = waitpid(adguard_pid, &status, WNOHANG); // non-blocking
        if (ret == -1) {
            perror("[ClearDNS] Waitpid error");
            server_exit(EXIT_WAIT_ERROR);
        } else if (ret) { // process exit
            adguard_pid = process_exec(adguard_command, FALSE);
            fprintf(stderr, "[ClearDNS] Catch AdGuardHome exit and restart it. (pid = %d -> %d)\n", ret, adguard_pid);
            sleep(wait_time); // reduce restart frequency
            return;
        }
    }

    if (overture_pid != 0) { // check overture
        ret = waitpid(overture_pid, &status, WNOHANG); // non-blocking
        if (ret == -1) {
            perror("[ClearDNS] Waitpid error");
            server_exit(EXIT_WAIT_ERROR);
        } else if (ret) { // process exit
            overture_pid = process_exec(overture_command, FALSE);
            fprintf(stderr, "[ClearDNS] Catch overture exit and restart it. (pid = %d -> %d)\n", ret, overture_pid);
            sleep(wait_time); // reduce restart frequency
            return;
        }
    }

    if (domestic_dnsproxy_pid != 0) { // check domestic dnsproxy
        ret = waitpid(domestic_dnsproxy_pid, &status, WNOHANG); // non-blocking
        if (ret == -1) {
            perror("[ClearDNS] Waitpid error");
            server_exit(EXIT_WAIT_ERROR);
        } else if (ret) { // process exit
            domestic_dnsproxy_pid = process_exec(domestic_dnsproxy_command, FALSE);
            fprintf(stderr, "[ClearDNS] Catch domestic dnsproxy exit and restart it. (pid = %d -> %d)\n", ret, domestic_dnsproxy_pid);
            sleep(wait_time); // reduce restart frequency
            return;
        }
    }

    if (foreign_dnsproxy_pid != 0) { // check foreign dnsproxy
        ret = waitpid(foreign_dnsproxy_pid, &status, WNOHANG); // non-blocking
        if (ret == -1) {
            perror("[ClearDNS] Waitpid error");
            server_exit(EXIT_WAIT_ERROR);
        } else if (ret) { // process exit
            foreign_dnsproxy_pid = process_exec(foreign_dnsproxy_command, FALSE);
            fprintf(stderr, "[ClearDNS] Catch foreign dnsproxy exit and restart it. (pid = %d -> %d)\n", ret, foreign_dnsproxy_pid);
            sleep(wait_time); // reduce restart frequency
            return;
        }
    }

    ret = waitpid(-1, &status, WNOHANG); // waitpid for all sub-process (non-blocking)
    if (ret == -1) {
        perror("[ClearDNS] Waitpid error");
        server_exit(EXIT_WAIT_ERROR);
    } else if (ret) { // process exit
        fprintf(stderr, "[ClearDNS] Catch subprocess exit. (pid = %d)\n", ret);
        return;
    }
    fprintf(stderr, "[ClearDNS] Subprocess not found.\n"); // get SIGCHLD signal but not exited subprocess found
}

pid_t process_exec(char **command, int is_new_group) {
    pid_t pid;
    char **env = {NULL};
    if ((pid = fork()) < 0) {
        perror("[ClearDNS] Fork error");
        server_exit(EXIT_FORK_ERROR);
    } else if (pid == 0) { // child process
        if (is_new_group) { setpgrp(); } // new process group
        prctl(PR_SET_PDEATHSIG, SIGKILL); // child process die with father process
        if (execvpe(command[0], command, env) < 0) {
            perror("[ClearDNS] Exec error");
            server_exit(EXIT_EXEC_ERROR);
        }
    }
    return pid;
}

void init_server(char *init_script, char *custom_script) { // run init script (blocking) / custom script (non-blocking)
    int pid, status;
    char *crond_command[] = {"crond", NULL};
    char *init_command[] = {"sh", init_script, NULL};
    char *custom_command[] = {"sh", custom_script, NULL};

    fprintf(stderr, "[ClearDNS] Start init process.\n");
    pid = process_exec(init_command, FALSE);
    wait(&status); // blocking wait
    fprintf(stderr, "[ClearDNS] Init complete. (pid = %d)\n", pid); // init process

    signal(SIGINT, get_exit_signal); // catch Ctrl + C (2)
    signal(SIGTERM, get_exit_signal); // catch exit signal (15)
    signal(SIGCHLD, get_sub_exit); // callback when child process die

    pid = process_exec(crond_command, FALSE);
    fprintf(stderr, "[ClearDNS] Crond load success. (pid = %d)\n", pid); // crond process

    if (access(custom_script, F_OK) >= 0) { // custom script exist
        custom_pgid = process_exec(custom_command, TRUE);
        fprintf(stderr, "[ClearDNS] Custom script is working. (pgid = %d)\n", custom_pgid); // custom process
        usleep(200000); // wait a moment (200ms)
    }
}

void server_daemon() { // run as a daemon of ClearDNS
    int wait_us_time = 200000; // 200ms

    adguard_pid = process_exec(adguard_command, FALSE);
    fprintf(stderr, "[ClearDNS] Exec AdGuardHome. (pid = %d)\n", adguard_pid);
    usleep(wait_us_time);

    overture_pid = process_exec(overture_command, FALSE);
    fprintf(stderr, "[ClearDNS] Exec overture. (pid = %d)\n", overture_pid);
    usleep(wait_us_time);

    domestic_dnsproxy_pid = process_exec(domestic_dnsproxy_command, FALSE);
    fprintf(stderr, "[ClearDNS] Exec domestic dnsproxy. (pid = %d)\n", domestic_dnsproxy_pid);
    usleep(wait_us_time);

    foreign_dnsproxy_pid = process_exec(foreign_dnsproxy_command, FALSE);
    fprintf(stderr, "[ClearDNS] Exec foreign dnsproxy. (pid = %d)\n", foreign_dnsproxy_pid);
    usleep(wait_us_time);

    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);
}
