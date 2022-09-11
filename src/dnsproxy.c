#include <stdlib.h>
#include <string.h>
#include "dnsproxy.h"
#include "logger.h"
#include "common.h"
#include "strList.h"

#include "cJSON.h"

dnsproxy* dnsproxy_init(int port) {
    dnsproxy *info = (dnsproxy*)malloc(sizeof(dnsproxy));
    info->port = port;
    info->verify = TRUE;
    info->parallel = TRUE;
    info->optimistic = FALSE;
    info->bootstrap = string_list_init();
    info->fallback = string_list_init();
    info->primary = string_list_init();
    info->cache = 4194304; // 4MiB
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
    log_debug("%s cache -> %d", caption, info->cache);
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

void dnsproxy_gen_config(dnsproxy *info) {
    cJSON *config = cJSON_CreateObject();
    if (!info->verify) {
        cJSON_AddTrueToObject(config, "insecure"); // insecure --(default)--> `false`
    }
    if (info->parallel) {
        cJSON_AddTrueToObject(config, "all-servers");
    }
    if (info->cache) {
        cJSON_AddTrueToObject(config, "cache");
        cJSON_AddNumberToObject(config, "cache-size", info->cache);
    }
    if (info->optimistic) {
        cJSON_AddTrueToObject(config, "cache-optimistic");
    }

    cJSON *port = cJSON_CreateArray();
    cJSON_AddItemToArray(port, cJSON_CreateNumber(info->port));
    cJSON_AddItemToObject(config, "listen-ports", port);

    cJSON *bootstrap = cJSON_CreateArray();
    for (char **server = info->bootstrap; *server != NULL; ++server) {
        cJSON_AddItemToArray(bootstrap, cJSON_CreateString(*server));
    }
    cJSON_AddItemToObject(config, "bootstrap", bootstrap);

    cJSON *fallback = cJSON_CreateArray();
    for (char **server = info->fallback; *server != NULL; ++server) {
        cJSON_AddItemToArray(fallback, cJSON_CreateString(*server));
    }
    cJSON_AddItemToObject(config, "fallback", fallback);

    cJSON *primary = cJSON_CreateArray();
    for (char **server = info->primary; *server != NULL; ++server) {
        cJSON_AddItemToArray(primary, cJSON_CreateString(*server));
    }
    cJSON_AddItemToObject(config, "upstream", primary);

    log_info("\n%s", cJSON_Print(config));
}
