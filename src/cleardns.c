#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "loader.h"
#include "logger.h"
#include "constant.h"
#include "dnsproxy.h"
#include "overture.h"
#include "structure.h"
#include "adguard.h"
#include "system.h"
#include "assets.h"
#include "sundry.h"

//char *init_script = "/usr/bin/load";
//char *custom_script = "/etc/cleardns/custom.sh";
//
//char *adguard_workdir = "/etc/cleardns/AdGuardHome";
//char *overture_config = "/etc/overture/config.yml";
//char *upstream_config = "/etc/cleardns/upstream.json";

#define HELP_MSG "\
ClearDNS usage \n\
--debug ...\n\
--config ...\n\
--help ...\n\
"

#define CONFIG_FILE "test.json"

// TODO: load `--debug`, `--config ...`, `--version`, `--help`

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


int main(int argc, char *argv[]) { // ClearDNS server

    char *config_file = init(argc, argv);

    LOG_LEVEL = LOG_DEBUG; // TODO: dev only
    log_info("ClearDNS server start (%s)", VERSION);

    load_config(config_file);
    free(config_file);

    dnsproxy_load("Domestic", loader.domestic, "domestic.json");
    dnsproxy_load("Foreign", loader.foreign, "foreign.json");
    overture_load(loader.diverter, "overture.json");
    adguard_load(loader.filter, ADGUARD_DIR);

    // TODO: running custom script


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
