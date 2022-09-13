#include <stdlib.h>
#include "common.h"
#include "logger.h"
#include "adguard.h"

adguard* adguard_init() {
    adguard *info = (adguard *)malloc(sizeof(adguard));
    char *port_str = uint32_to_string(DIVERTER_PORT);
    info->debug = FALSE;
    info->dns_port = DNS_PORT;
    info->web_port = ADGUARD_PORT;
    info->upstream = string_join("127.0.0.1:", port_str);
    info->username = string_init(ADGUARD_USER);
    info->password = string_init(ADGUARD_PASSWD);
    free(port_str);
    return info;
}

void adguard_dump(adguard *info) { // show adguard info in debug log
    log_debug("AdGuardHome debug -> %s", show_bool(info->debug));
    log_debug("AdGuardHome dns port -> %u", info->dns_port);
    log_debug("AdGuardHome web port -> %u", info->web_port);
    log_debug("AdGuardHome upstream -> %s", info->upstream);
    log_debug("AdGuardHome username -> %s", info->username);
    log_debug("AdGuardHome password -> %s", info->password);
}

process* adguard_load(adguard *info, const char *dir) {
    adguard_dump(info);
    if (!check_port(info->dns_port)) { // invalid dns port
        log_fatal("Invalid dns port `%u`", info->dns_port);
    }
    if (!check_port(info->web_port)) { // invalid web port
        log_fatal("Invalid web port `%u`", info->web_port);
    }

    // TODO: modify configure file
    // inject -> dns_port / upstream / username / password

    process *proc = process_init("AdGuardHome", ADGUARD_BIN);
    char *port_str = uint32_to_string(info->web_port);
    process_add_arg(proc, "--work-dir");
    process_add_arg(proc, dir);
    process_add_arg(proc, "--port");
    process_add_arg(proc, port_str);
    if (info->debug) {
        process_add_arg(proc, "--verbose"); // adguard enable debug mode
    }
    free(port_str);
    return proc;
}
