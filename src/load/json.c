#include <string.h>
#include <stdlib.h>
#include "load.h"
#include "cJSON.h"
#include "common.h"
#include "logger.h"
#include "structure.h"

int json_int_value(char *key, cJSON *json) { // json int or string value -> int
    if (cJSON_IsNumber(json)) {
        return json->valueint;
    } else if (cJSON_IsString(json)) {
        char *p;
        int int_ret = (int)strtol(json->valuestring, &p, 10);
        if (int_ret == 0 && strcmp(json->valuestring, "0") != 0) { // invalid number in string
            log_fatal("`%s` not a valid number", key);
        }
        return int_ret;
    } else {
        log_fatal("`%s` must be number or string", key);
    }
    return 0; // never reach
}

char* json_string_value(char* key, cJSON *json) {
    if (!cJSON_IsString(json)) {
        log_fatal("`%s` must be string", key);
    }
    return json->valuestring;
}

char** json_string_list_value(char *key, cJSON *json, char **string_list) {
    if (cJSON_IsString(json)) {
        string_list = string_list_append(string_list, json->valuestring);
    } else if (cJSON_IsArray(json)) {
        json = json->child;
        while (json != NULL) {
            if (!cJSON_IsString(json)) {
                log_fatal("`%s` must be string array", key);
            }
            string_list = string_list_append(string_list, json->valuestring);
            json = json->next; // next field
        }
    } else {
        log_fatal("`%s` must be array or string", key);
    }
    return string_list;
}

uint32_t** json_uint32_list_value(char *key, cJSON *json, uint32_t **uint32_list) {
    if (cJSON_IsNumber(json)) {
        uint32_list = uint32_list_append(uint32_list, json->valueint);
    } else if (cJSON_IsArray(json)) {
        json = json->child;
        while (json != NULL) {
            if (!cJSON_IsNumber(json)) {
                log_fatal("`%s` must be number array", key);
            }
            uint32_list = uint32_list_append(uint32_list, json->valueint);
            json = json->next; // next field
        }
    } else {
        log_fatal("`%s` must be array or number", key);
    }
    return uint32_list;
}

uint8_t json_bool_value(char *key, cJSON *json) { // json bool value -> bool
    if (!cJSON_IsBool(json)) {
        log_fatal("`%s` must be boolean", key);
    }
    return json->valueint;
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

void json_diverter_parser(diverter_config *config, cJSON *json) { // diverter options parser
    while (json != NULL) {
        if (!strcmp(json->string, "port")) {
            config->port = json_int_value("diverter.port", json);
        }
        if (!strcmp(json->string, "gfwlist")) {
            config->gfwlist = json_string_list_value("diverter.gfwlist", json, config->gfwlist);
        }
        if (!strcmp(json->string, "china-ip")) {
            config->china_ip = json_string_list_value("diverter.china-ip", json, config->china_ip);
        }
        if (!strcmp(json->string, "chinalist")) {
            config->chinalist = json_string_list_value("diverter.chinalist", json, config->chinalist);
        }
        json = json->next; // next field
    }
}

void json_adguard_parser(adguard_config *config, cJSON *json) { // adguard options parser
    while (json != NULL) {
        if (!strcmp(json->string, "port")) {
            config->port = json_int_value("adguard.port", json);
        }
        if (!strcmp(json->string, "enable")) {
            config->enable = json_bool_value("adguard.enable", json);
        }
        if (!strcmp(json->string, "username")) {
            config->username = json_string_value("adguard.username", json);
        }
        if (!strcmp(json->string, "password")) {
            config->password = json_string_value("adguard.password", json);
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

        if (!strcmp(json->string, "diverter")) {
            json_diverter_parser(&config->diverter, json->child);
        }
        if (!strcmp(json->string, "adguard")) {
            json_adguard_parser(&config->adguard, json->child);
        }
        if (!strcmp(json->string, "reject")) {
            config->reject = json_uint32_list_value("reject", json, config->reject);
        }
        if (!strcmp(json->string, "hosts")) {
            config->hosts = json_string_list_value("hosts", json, config->hosts);
        }
        if (!strcmp(json->string, "ttl")) {
            config->ttl = json_string_list_value("ttl", json, config->ttl);
        }
        json = json->next; // next field
    }
    cJSON_free(json); // free JSON struct
}
