#include <string.h>
#include <stdlib.h>
#include "load.h"
#include "cJSON.h"
#include "common.h"
#include "logger.h"

int json_int_value(char *key, cJSON *json) { // json int or string value -> int
    if (cJSON_IsNumber(json)) {
        return json->valueint;
    } else if (cJSON_IsString(json)) {
        char *p;
        return (int)strtol(json->valuestring, &p, 10);
    } else {
        log_fatal("`%s` must be number or string", key);
    }
    return 0; // never reach
}

uint8_t json_bool_value(char *key, cJSON *json) { // json bool value -> bool
    if (cJSON_IsBool(json)) {
        return json->valueint;
    } else {
        log_fatal("`%s` must be boolean", key);
    }
    return FALSE;
}

void json_cache_parser(cache_config *config, cJSON *json) { // cache options parser
    while (json != NULL) {
        if (!strcmp(json->string, "size")) {
            config->size = json_int_value("cache.size", json);
        }
        if (!strcmp(json->string, "enable")) {
            config->enable = json_bool_value("cache.enable", json);
        }
        if (!strcmp(json->string, "optimistic")) {
            config->optimistic = json_bool_value("cache.optimistic", json);
        }
        json = json->next; // next field
    }
}

void json_config_parser(cleardns_config *config, const char *config_file) { // JSON format configure
    char *config_content = read_file(config_file);
    cJSON *json = cJSON_Parse(config_content);
    if (json == NULL) {
        log_fatal("JSON configure `%s` format error", config_file);
    }
    json = json->child;
    while (json != NULL) {
        if (!strcmp(json->string, "port")) {
            config->port = json_int_value("port", json);
        }
        if (!strcmp(json->string, "cache")) {
            json_cache_parser(&config->cache, json->child);
        }

        // domestic
        // foreign

        // diverter
        // adguard
        // reject
        // hosts
        // ttl

        json = json->next; // next field
    }
    cJSON_free(json); // free JSON struct
}
