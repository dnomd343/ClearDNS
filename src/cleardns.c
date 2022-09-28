#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "loader.h"
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "assets.h"
#include "adguard.h"
#include "crontab.h"
#include "constant.h"
#include "dnsproxy.h"
#include "overture.h"
#include "structure.h"

struct {
    char *config;
    uint8_t debug;
    uint8_t verbose;
} settings;

void init(int argc, char *argv[]) { // return config file
    settings.config = string_init(CONFIG_FILE);
    settings.debug = FALSE;
    settings.verbose = FALSE;

    if (getenv("CONFIG") != NULL) {
        free(settings.config);
        settings.config = string_init(getenv("CONFIG"));
    }
    if (getenv("DEBUG") != NULL && !strcmp(getenv("DEBUG"), "TRUE")) {
        settings.debug = TRUE;
    }
    if (getenv("VERBOSE") != NULL && !strcmp(getenv("VERBOSE"), "TRUE")) {
        settings.verbose = TRUE;
    }

    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "--debug")) {
            settings.debug = TRUE;
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
            free(settings.config);
            settings.config = string_init(argv[++i]); // use custom config file
        }
    }
    log_debug("Config file -> %s", settings.config);
}

void cleardns() { // cleardns service
    if (settings.verbose || settings.debug) {
        LOG_LEVEL = LOG_DEBUG; // enable debug log level
    }
    create_folder(EXPOSE_DIR);
    create_folder(WORK_DIR);
    chdir(EXPOSE_DIR);
    load_config(settings.config); // configure parser
    free(settings.config);
    if (settings.debug) { // debug mode enabled
        loader.diverter->debug = TRUE;
        loader.domestic->debug = TRUE;
        loader.foreign->debug = TRUE;
        if (loader.crond != NULL) {
            loader.crond->debug = TRUE;
        }
        if (loader.filter != NULL) {
            loader.filter->debug = TRUE;
        }
    }

    log_info("Start loading process");
    process_list_init();
    assets_load(loader.resource);
    process_list_append(dnsproxy_load("Domestic", loader.domestic, "domestic.json"));
    process_list_append(dnsproxy_load("Foreign", loader.foreign, "foreign.json"));
    process_list_append(overture_load(loader.diverter, "overture.json"));
    overture_free(loader.diverter);
    dnsproxy_free(loader.domestic);
    dnsproxy_free(loader.foreign);
    assets_free(loader.resource);
    if (loader.crond != NULL) {
        process_list_append(crontab_load(loader.crond)); // free crontab struct later
    }
    if (loader.filter != NULL) {
        process_list_append(adguard_load(loader.filter, ADGUARD_DIR));
        adguard_free(loader.filter);
    }

    for (char **script = loader.script; *script != NULL; ++script) { // running custom script
        log_info("Run custom script -> `%s`", *script);
        run_command(*script);
    }
    string_list_free(loader.script);

    process_list_run(); // start all process
    if (loader.crond != NULL) { // assets not disabled
        pid_t my_pid = getpid();
        log_info("ClearDNS PID -> %d", my_pid);
        kill(my_pid, SIGALRM); // send alarm signal to itself
        crontab_free(loader.crond);
    }
    process_list_daemon(); // daemon all process
}

int main(int argc, char *argv[]) {
    init(argc, argv);
    log_info("ClearDNS server start (%s)", VERSION);
    cleardns();
    return 0;
}
