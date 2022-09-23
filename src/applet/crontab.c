#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include "config.h"
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "process.h"
#include "structure.h"

char **update_file;
char **update_url;

void assets_update() { // update all assets
    log_info("Start assets update");
    for (char **file = update_file; *file != NULL; ++file) {
        char **url = file - update_file + update_url;
        log_info("Update asset `%s` -> %s", *file, *url);
        download_file(*file, *url); // download asset from url
    }
    log_info("Restart overture");
    run_command("pgrep overture | xargs kill"); // restart overture
    log_info("Assets update complete");
}

void assets_free(assets_config *info) { // free assets config
    string_list_free(info->update_file);
    string_list_free(info->update_url);
    free(info->cron);
    free(info);
}

process* assets_load(assets_config *info) { // load assets update options
    update_url = string_list_init();
    update_file = string_list_init();
    string_list_update(&update_url, info->update_url);
    string_list_update(&update_file, info->update_file);
    signal(SIGALRM, assets_update); // receive SIGALRM signal

    char *cron = string_join(info->cron, " kill -14 1");
    save_file("/var/spool/cron/crontabs/root", cron);
    free(cron);

    process *proc = process_init("Crontab", "crond");
    process_add_arg(proc, "-f"); // foreground
    return proc;
}
