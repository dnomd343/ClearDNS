#include <stdlib.h>
#include "process.h"
#include "config.h"
#include "logger.h"
#include "sundry.h"
#include "system.h"

process* assets_load(assets_config *info) { // load assets update options
    log_info("assets test ok");

    // TODO: update assets process
    // TODO: save cron exp in `/var/spool/cron/crontabs/root`

    char *cron = string_join(info->cron, " kill -14 1");

    log_info("Crontab exp -> `%s`", cron);

    save_file("/var/spool/cron/crontabs/root", cron);

    free(cron);

    return process_init("Crontab", "crond");
}
