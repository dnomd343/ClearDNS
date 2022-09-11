#include <stdlib.h>
#include "overture.h"
#include "common.h"
#include "logger.h"

overture* overture_init(int port) { // init overture options
    overture *info = (overture*)malloc(sizeof(overture));
    info->port = port;
    info->timeout = 6; // default timeout -> 6s
    info->foreign_port = FOREIGN_PORT;
    info->domestic_port = DOMESTIC_PORT;
    info->foreign_ip_file = "/dev/null";
    info->domestic_ip_file = "/dev/null";
    info->foreign_domain_file = "/dev/null";
    info->domestic_domain_file = "/dev/null";
    return info;
}

void overture_dump(overture *info) { // show overture info in debug log
    log_debug("Overture port -> %d", info->port);
    log_debug("Overture timeout -> %d", info->timeout);
    log_debug("Overture foreign port -> %d", info->foreign_port);
    log_debug("Overture domestic port -> %d", info->domestic_port);
    log_debug("Overture foreign ip file -> %s", info->foreign_ip_file);
    log_debug("Overture domestic ip file -> %s", info->domestic_ip_file);
    log_debug("Overture foreign domain file -> %s", info->foreign_domain_file);
    log_debug("Overture domestic domain file -> %s", info->domestic_domain_file);
}
