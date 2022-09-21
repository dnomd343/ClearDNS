#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "loader.h"
#include "logger.h"
#include "constant.h"
#include "dnsproxy.h"
#include "overture.h"
#include "adguard.h"
#include "system.h"
#include "sundry.h"
#include "structure.h"

char* init(int argc, char *argv[]) { // return config file
    char *config = string_init(CONFIG_FILE);
    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "--debug")) {
            LOG_LEVEL = LOG_DEBUG; // enable debug mode
        }
        if (!strcmp(argv[i], "--version")) {
            printf("ClearDNS version %s\n", VERSION); // show version
            exit(0);
        }
        if (!strcmp(argv[i], "--help")) {
            printf("\n%s\n", HELP_MSG); // show help message
            exit(0);
        }
        if (!strcmp(argv[i], "--config")) {
            if (i + 1 == argc) {
                log_error("Option `--config` missing value");
                exit(1);
            }
            free(config);
            config = string_init(argv[++i]); // use custom config file
        }
    }
    log_debug("Config file -> %s", config);
    return config;
}

int main(int argc, char *argv[]) { // ClearDNS service

    char *config_file = init(argc, argv);

    LOG_LEVEL = LOG_DEBUG;
    log_info("ClearDNS server start (%s)", VERSION);

//    char **test = string_list_init();
//    log_info("len -> %u", string_list_len(test));
//    string_list_append(&test, "test1");
//    string_list_append(&test, "test2");
//    string_list_append(&test, "test3");
//    log_info("len -> %u", string_list_len(test));
//
//    char *temp = string_list_dump(test);
//    log_info("content -> %s", temp);
//    free(temp);
//
//    char **add = string_list_init();
//    string_list_append(&add, "test4");
//    string_list_append(&add, "test5");
//
//    string_list_update(&test, add);
//    log_info("len -> %d", string_list_len(test));
//    temp = string_list_dump(test);
//    log_info("content -> %s", temp);
//    free(temp);


    uint32_t **test = uint32_list_init();
    log_info("len -> %u", uint32_list_len(test));
    uint32_list_append(&test, 1);
    uint32_list_append(&test, 2);
    uint32_list_append(&test, 3);
    log_info("len -> %u", uint32_list_len(test));

    char *temp = uint32_list_dump(test);
    log_info("content -> %s", temp);
    free(temp);

    uint32_t **add = uint32_list_init();
    uint32_list_append(&add, 4);
    uint32_list_append(&add, 5);

    uint32_list_update(&test, add);
    log_info("len -> %d", uint32_list_len(test));
    temp = uint32_list_dump(test);
    log_info("content -> %s", temp);
    free(temp);


//    process *test = process_init("TEST", "lls");
//    process *test = process_init("TEST", "ls");
//    process_add_arg(test, "-al");
//    test->cwd = "/etc/cleardns/fuck";
//    int pid = process_exec(test);
//    log_info("PID = %d", pid);

//    int status;
//    wait(&status);
//    return 0;

//    create_folder(WORK_DIR);

    // TODO: load assets first
//    load_config(config_file);
//    free(config_file);

//    process_list_init();
//    process_list_append(dnsproxy_load("Domestic", loader.domestic, "domestic.json"));
//    process_list_append(dnsproxy_load("Foreign", loader.foreign, "foreign.json"));
//    process_list_append(overture_load(loader.diverter, "overture.json"));
//    if (loader.filter != NULL) {
//        process_list_append(adguard_load(loader.filter, ADGUARD_DIR));
//    }

    // TODO: running custom script

//    process_list_run();


//    init_server(init_script, custom_script); // run init script and custom script
//
//    load_start_command(adguard_workdir, overture_config, upstream_config, debug_mode); // generate commands
//    if (debug_mode) { // show exec command
//        fprintf(stderr, "[ClearDNS] Debug mode.\n");
//        show_command("[ClearDNS] adguard", adguard_command);
//        show_command("[ClearDNS] overture", overture_command);
//        show_command("[ClearDNS] dnsproxy (domestic)", domestic_dnsproxy_command);
//        show_command("[ClearDNS] dnsproxy (foreign)", foreign_dnsproxy_command);
//    }
//
//    server_daemon(); // run as daemon to manage process in docker
    return 0;
}
