#include <stdlib.h>
#include "cJSON.h"
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "constant.h"
#include "dnsproxy.h"
#include "structure.h"

char* dnsproxy_config(dnsproxy *info);
void dnsproxy_dump(const char *caption, dnsproxy *info);

void dnsproxy_add_primary(dnsproxy *info, const char *server) { // add primary dns server
    info->primary = string_list_append(info->primary, server);
}

void dnsproxy_add_fallback(dnsproxy *info, const char *server) { // add fallback dns server
    info->fallback = string_list_append(info->fallback, server);
}

void dnsproxy_add_bootstrap(dnsproxy *info, const char *server) { // add bootstrap dns server
    info->bootstrap = string_list_append(info->bootstrap, server);
}

void dnsproxy_free(dnsproxy *info) { // free dnsproxy options
    string_list_free(info->bootstrap);
    string_list_free(info->fallback);
    string_list_free(info->primary);
    free(info);
}

dnsproxy* dnsproxy_init(uint16_t port) { // init dnsproxy options
    dnsproxy *info = (dnsproxy *)malloc(sizeof(dnsproxy));
    info->port = port;
    info->cache = 0; // disable cache in default
    info->debug = FALSE;
    info->verify = TRUE;
    info->parallel = TRUE;
    info->optimistic = FALSE;
    info->bootstrap = string_list_init();
    info->fallback = string_list_init();
    info->primary = string_list_init();
    return info;
}

void dnsproxy_dump(const char *caption, dnsproxy *info) { // show dnsproxy options in debug log
    char *str_dump;
    log_debug("%s port -> %u", caption, info->port);
    log_debug("%s cache -> %u", caption, info->cache);
    log_debug("%s debug -> %s", caption, show_bool(info->debug));
    log_debug("%s verify -> %s", caption, show_bool(info->verify));
    log_debug("%s parallel -> %s", caption, show_bool(info->parallel));
    log_debug("%s optimistic -> %s", caption, show_bool(info->optimistic));

    str_dump = string_list_dump(info->bootstrap);
    log_debug("%s bootstrap -> %s", caption, str_dump);
    free(str_dump);

    str_dump = string_list_dump(info->fallback);
    log_debug("%s fallback -> %s", caption, str_dump);
    free(str_dump);

    str_dump = string_list_dump(info->primary);
    log_debug("%s primary -> %s", caption, str_dump);
    free(str_dump);
}

process* dnsproxy_load(const char *caption, dnsproxy *info, const char *file) { // load dnsproxy options
    dnsproxy_dump(caption, info);
    if (!check_port(info->port)) { // invalid server port
        log_fatal("Invalid port `%u`", info->port);
    }
    if (string_list_len(info->primary) == 0) { // without primary dns server
        log_fatal("%s without primary dns server", caption);
    }

    create_folder(WORK_DIR); // ensure work dir exist
    char *config = dnsproxy_config(info); // string config with json format
    char *config_file = string_join(WORK_DIR, file);
    save_file(config_file, config); // load dnsproxy configure
    free(config_file);
    free(config);

    char *option = string_join("--config-path=", file);
    process *proc = process_init(caption, DNSPROXY_BIN); // generate dnsproxy command
    process_add_arg(proc, option);
    if (info->debug) {
        process_add_arg(proc, "--verbose"); // dnsproxy enable debug mode
    }
    free(option);
    log_info("%s load success", caption);
    return proc;
}

char* dnsproxy_config(dnsproxy *info) { // generate json configure from dnsproxy options
    cJSON *config = cJSON_CreateObject();
    if (!info->verify) {
        cJSON_AddTrueToObject(config, "insecure"); // insecure --(default)--> `false`
    }
    if (info->parallel) {
        cJSON_AddTrueToObject(config, "all-servers"); // all-servers --(default)--> `false`
    }
    if (info->cache) {
        cJSON_AddTrueToObject(config, "cache"); // cache --(default)--> `false`
        cJSON_AddNumberToObject(config, "cache-size", info->cache);
    }
    if (info->optimistic) {
        cJSON_AddTrueToObject(config, "cache-optimistic"); // cache-optimistic --(default)--> `false`
    }

    cJSON *port = cJSON_CreateArray();
    cJSON_AddItemToArray(port, cJSON_CreateNumber(info->port));
    cJSON_AddItemToObject(config, "listen-ports", port); // listen-ports <=> port

    cJSON *bootstrap = cJSON_CreateArray();
    for (char **server = info->bootstrap; *server != NULL; ++server) {
        cJSON_AddItemToArray(bootstrap, cJSON_CreateString(*server));
    }
    cJSON_AddItemToObject(config, "bootstrap", bootstrap); // bootstrap <=> bootstrap

    cJSON *fallback = cJSON_CreateArray();
    for (char **server = info->fallback; *server != NULL; ++server) {
        cJSON_AddItemToArray(fallback, cJSON_CreateString(*server));
    }
    cJSON_AddItemToObject(config, "fallback", fallback); // fallback <=> fallback

    cJSON *primary = cJSON_CreateArray();
    for (char **server = info->primary; *server != NULL; ++server) {
        cJSON_AddItemToArray(primary, cJSON_CreateString(*server));
    }
    cJSON_AddItemToObject(config, "upstream", primary); // upstream <=> primary

    char *config_str = cJSON_Print(config);
    cJSON_Delete(config); // free json object
    return config_str;
}
