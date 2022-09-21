#include "process.h"
#include "config.h"
#include "logger.h"

process* assets_load(assets_config *info) { // load assets update options
    log_info("assets test ok");

    // TODO: update assets process
    // TODO: save cron exp in `/var/spool/cron/crontabs/root`

    return process_init("Crontab", "crond");
}
