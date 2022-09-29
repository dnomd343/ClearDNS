#include <stdlib.h>
#include <unistd.h>
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

crontab* crontab_init() { // init crontab options
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
    crontab_dump(info);
    char *my_pid = uint32_to_string(getpid());
    char *cron_cmd = string_join("\tkill -14 ", my_pid); // SIGALRM -> 14
    char *cron_exp = string_join(info->cron, cron_cmd);
    save_file("/var/spool/cron/crontabs/root", cron_exp);
    free(cron_exp);
    free(cron_cmd);
    free(my_pid);

    process *proc = process_init("Crontab", "crond");
    process_add_arg(proc, "-f"); // foreground
    if (info->debug) {
        process_add_arg(proc, "-l");
        process_add_arg(proc, "0"); // verbose mode
    }
    return proc;
}
