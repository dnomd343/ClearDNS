#include <stdlib.h>
#include "dnsproxy.h"
#include "logger.h"
#include "common.h"
#include "strList.h"

dnsproxy* dnsproxy_init(int port) {
    dnsproxy *info = (dnsproxy*)malloc(sizeof(dnsproxy));
    info->port = port;
    info->verify = TRUE;
    info->parallel = TRUE;
    info->optimistic = FALSE;
    info->bootstrap = string_list_init();
    info->fallback = string_list_init();
    info->primary = string_list_init();
    info->cache = "4194304"; // 4MiB
    return info;
}

void dnsproxy_dump(char *caption, dnsproxy *info) {
    log_debug("%s listen port -> %d", caption, info->port);
    log_debug("%s verify -> %s", caption, show_bool(info->verify));
    log_debug("%s parallel -> %s", caption, show_bool(info->parallel));
    log_debug("%s optimistic -> %s", caption, show_bool(info->optimistic));
    log_debug("%s bootstrap -> %s", caption, string_list_dump(info->bootstrap));
    log_debug("%s fallback -> %s", caption, string_list_dump(info->fallback));
    log_debug("%s primary -> %s", caption, string_list_dump(info->primary));
    log_debug("%s cache -> %s", caption, info->cache);
}

void dnsproxy_add_primary(dnsproxy *info, char *server) {
    info->primary = string_list_append(info->primary, server);
}

void dnsproxy_add_fallback(dnsproxy *info, char *server) {
    info->fallback = string_list_append(info->fallback, server);
}

void dnsproxy_add_bootstrap(dnsproxy *info, char *server) {
    info->bootstrap = string_list_append(info->bootstrap, server);
}
