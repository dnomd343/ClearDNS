#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "bcrypt.h"
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "adguard.h"
#include "constant.h"

void adguard_dump(adguard *info);
char *adguard_config(adguard *info, const char *raw_config);

void adguard_free(adguard *info) { // free adguard options
    free(info->upstream);
    free(info->username);
    free(info->password);
    free(info);
}

adguard* adguard_init() { // init adguard options
    adguard *info = (adguard *)malloc(sizeof(adguard));
    char *port_str = uint32_to_string(DIVERTER_PORT); // diverter port in string
    info->debug = FALSE;
    info->dns_port = DNS_PORT;
    info->web_port = ADGUARD_PORT;
    info->upstream = string_join("127.0.0.1:", port_str); // default upstream
    info->username = strdup(ADGUARD_USER);
    info->password = strdup(ADGUARD_PASSWD);
    free(port_str);
    return info;
}

void adguard_dump(adguard *info) { // show adguard options in debug log
    log_debug("AdGuardHome debug -> %s", show_bool(info->debug));
    log_debug("AdGuardHome dns port -> %u", info->dns_port);
    log_debug("AdGuardHome web port -> %u", info->web_port);
    log_debug("AdGuardHome upstream -> %s", info->upstream);
    log_debug("AdGuardHome username -> %s", info->username);
    log_debug("AdGuardHome password -> %s", info->password);
}

char *adguard_config(adguard *info, const char *raw_config) { // modify adguard configure
    cJSON *json = cJSON_Parse(raw_config);
    if (json == NULL) {
        log_fatal("AdGuardHome configure error");
    }

    cJSON *user_config = cJSON_CreateObject(); // setting up username and password
    cJSON *users_config = cJSON_CreateArray();
    char *password = bcrypt_cal(info->password);
    cJSON_AddItemToObject(user_config, "name", cJSON_CreateString(info->username));
    cJSON_AddItemToObject(user_config, "password", cJSON_CreateString(password));
    cJSON_AddItemToArray(users_config, user_config);
    json_field_replace(json, "users", users_config);
    free(password);

    cJSON *dns = json_field_get(json, "dns"); // setting up dns options
    cJSON *upstream = cJSON_CreateArray();
    cJSON_AddItemToArray(upstream, cJSON_CreateString(info->upstream));
    json_field_replace(dns, "port", cJSON_CreateNumber(info->dns_port));
    json_field_replace(dns, "bind_host", cJSON_CreateString("0.0.0.0"));
    json_field_replace(dns, "upstream_dns", upstream);
    json_field_replace(dns, "upstream_dns_file", cJSON_CreateString(""));
    json_field_replace(dns, "bootstrap_dns", cJSON_CreateArray());

    char *config = cJSON_Print(json); // generate json string
    cJSON_free(json);
    return config;
}

process* adguard_load(adguard *info, const char *dir) { // load adguard options
    adguard_dump(info);
    if (!check_port(info->dns_port)) { // invalid dns port
        log_fatal("Invalid dns port `%u`", info->dns_port);
    }
    if (!check_port(info->web_port)) { // invalid web port
        log_fatal("Invalid web port `%u`", info->web_port);
    }
    if (!strcmp(info->username, "")) { // invalid username
        log_fatal("Invalid AdGuardHome username");
    }
    if (!strcmp(info->password, "")) { // invalid password
        log_fatal("Invalid AdGuardHome password");
    }

    char *adguard_config_ret;
    char *adguard_config_file = string_join(dir, "AdGuardHome.yaml");
    create_folder(dir); // ensure adguard work dir exist

    if (!is_file_exist(adguard_config_file)) { // AdGuardHome configure not exist
        log_info("Create AdGuardHome configure");
        adguard_config_ret = adguard_config(info, "{}"); // begin with empty json
    } else { // configure exist -> modify
        char *adguard_config_content = read_file(adguard_config_file);
        char *adguard_config_json = to_json(adguard_config_content);
        log_debug("AdGuardHome json configure ->\n%s", adguard_config_json);
        adguard_config_ret = adguard_config(info, adguard_config_json);
        free(adguard_config_content);
        free(adguard_config_json);
    }
    save_file(adguard_config_file, adguard_config_ret); // save modified configure
    free(adguard_config_file);

    process *proc = process_init("AdGuardHome", ADGUARD_BIN); // generate adguard command
    char *port_str = uint32_to_string(info->web_port);
    process_add_arg(proc, "--no-check-update"); // disable update check (invalid in docker)
    process_add_arg(proc, "--work-dir");
    process_add_arg(proc, dir);
    process_add_arg(proc, "--port"); // web server port
    process_add_arg(proc, port_str);
    if (info->debug) {
        process_add_arg(proc, "--verbose"); // adguard enable debug mode
    }
    free(port_str);
    log_info("AdGuardHome load success");
    return proc;
}
