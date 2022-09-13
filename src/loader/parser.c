#include <string.h>
#include <stdlib.h>
#include "cJSON.h"
#include "common.h"
#include "logger.h"
#include "config.h"
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
    } else if (!cJSON_IsNull(json)) {
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
    } else if (!cJSON_IsNull(json)) {
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
    if (!cJSON_IsObject(json)) {
        log_fatal("`cache` must be object");
    }
    json = json->child;
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

void json_upstream_parser(char *caption, upstream_config *config, cJSON *json) {
    if (!cJSON_IsObject(json)) {
        log_fatal("`%s` must be object", caption);
    }
    char *key_name;
    json = json->child;
    while (json != NULL) {
        if (!strcmp(json->string, "port")) {
            key_name = string_join(caption, ".port");
            config->port = json_int_value(key_name, json);
            free(key_name);
        }
        if (!strcmp(json->string, "verify")) {
            key_name = string_join(caption, ".verify");
            config->verify = json_bool_value(key_name, json);
            free(key_name);
        }
        if (!strcmp(json->string, "parallel")) {
            key_name = string_join(caption, ".parallel");
            config->parallel = json_bool_value(key_name, json);
            free(key_name);
        }
        if (!strcmp(json->string, "bootstrap")) {
            key_name = string_join(caption, ".bootstrap");
            config->bootstrap = json_string_list_value(key_name, json, config->bootstrap);
            free(key_name);
        }
        if (!strcmp(json->string, "fallback")) {
            key_name = string_join(caption, ".fallback");
            config->fallback = json_string_list_value(key_name, json, config->fallback);
            free(key_name);
        }
        if (!strcmp(json->string, "primary")) {
            key_name = string_join(caption, ".primary");
            config->primary = json_string_list_value(key_name, json, config->primary);
            free(key_name);
        }
        json = json->next; // next field
    }
}

void json_diverter_parser(diverter_config *config, cJSON *json) { // diverter options parser
    if (!cJSON_IsObject(json)) {
        log_fatal("`diverter` must be object");
    }
    json = json->child;
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
    if (!cJSON_IsObject(json)) {
        log_fatal("`adguard` must be array");
    }
    json = json->child;
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

void json_config_parser(cleardns_config *config, const char *config_content) { // JSON format configure
    cJSON *json = cJSON_Parse(config_content);
    if (json == NULL) {
        log_fatal("JSON configure format error");
    }
    json = json->child;
    while (json != NULL) {
        if (!strcmp(json->string, "port")) {
            config->port = json_int_value("port", json);
        }
        if (!strcmp(json->string, "cache")) {
            json_cache_parser(&config->cache, json);
        }
        if (!strcmp(json->string, "domestic")) {
            json_upstream_parser("domestic", &config->domestic, json);
        }
        if (!strcmp(json->string, "foreign")) {
            json_upstream_parser("foreign", &config->foreign, json);
        }
        if (!strcmp(json->string, "diverter")) {
            json_diverter_parser(&config->diverter, json);
        }
        if (!strcmp(json->string, "adguard")) {
            json_adguard_parser(&config->adguard, json);
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

void yaml_config_parser(cleardns_config *config, const char *config_content) { // YAML format configure
    // TODO: change YAML -> JSON
    char *json_content = string_init(config_content); // just demo for now

    json_config_parser(config, json_content);
}

uint8_t is_json_suffix(const char *file_name) {
    if (strlen(file_name) <= 5) { // `.json`
        return FALSE;
    }
    if (!strcmp(file_name + strlen(file_name) - 5, ".json")) {
        return TRUE;
    }
    return FALSE;
}

void config_parser(cleardns_config *config, const char *config_file) {
    char *config_content = read_file(config_file);
    if (is_json_suffix(config_file)) {
        log_info("Start JSON configure parser");
        json_config_parser(config, config_content);
    } else {
        log_info("Start YAML configure parser");
        yaml_config_parser(config, config_content);
    }
    log_info("Configure parser success");
}
