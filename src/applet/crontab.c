#include <stdlib.h>
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "crontab.h"
#include "process.h"
#include "constant.h"

void crontab_dump(crontab *info);

void crontab_free(crontab *info) { // free crontab options
    free(info->cron);
    free(info);
}

crontab* crontab_init(char *cron) { // init crontab options
    crontab *info = (crontab *)malloc(sizeof(crontab));
    info->debug = FALSE;
    info->cron = string_init(UPDATE_CRON);
    return info;
}

void crontab_dump(crontab *info) { // show crontab options in debug log
    log_debug("Crontab debug -> %s", show_bool(info->debug));
    log_debug("Crontab expression -> `%s`", info->cron);
}

process* crontab_load(crontab *info) { // load crontab options
    // TODO: avoid the case that PID of cleardns is not 1 (docker --pid)
    char *cron_cmd = string_join(info->cron, " kill -14 1"); // SIGALRM -> 14
    save_file("/var/spool/cron/crontabs/root", cron_cmd);
    free(cron_cmd);

    process *proc = process_init("Crontab", "crond");
    process_add_arg(proc, "-f"); // foreground
    if (info->debug) {
        process_add_arg(proc, "-l");
        process_add_arg(proc, "0"); // verbose mode
    }
    return proc;
}
