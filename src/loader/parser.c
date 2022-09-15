#include <string.h>
#include <stdlib.h>
#include "json.h"
#include "config.h"
#include "logger.h"
#include "sundry.h"
#include "system.h"

void cache_parser(cache_config *config, cJSON *json) { // cache options parser
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

void upstream_parser(char *caption, upstream_config *config, cJSON *json) { // upstream options parser
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

void diverter_parser(diverter_config *config, cJSON *json) { // diverter options parser
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

void adguard_parser(adguard_config *config, cJSON *json) { // adguard options parser
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
            free(config->username);
            config->username = json_string_value("adguard.username", json);
        }
        if (!strcmp(json->string, "password")) {
            free(config->password);
            config->password = json_string_value("adguard.password", json);
        }
        json = json->next; // next field
    }
}

void cleardns_parser(cleardns_config *config, const char *config_content) { // JSON format configure
    cJSON *json = cJSON_Parse(config_content);
    if (json == NULL) {
        log_fatal("ClearDNS configure format error");
    }
    json = json->child;
    while (json != NULL) {
        if (!strcmp(json->string, "port")) {
            config->port = json_int_value("port", json);
        }
        if (!strcmp(json->string, "cache")) {
            cache_parser(&config->cache, json);
        }
        if (!strcmp(json->string, "domestic")) {
            upstream_parser("domestic", &config->domestic, json);
        }
        if (!strcmp(json->string, "foreign")) {
            upstream_parser("foreign", &config->foreign, json);
        }
        if (!strcmp(json->string, "diverter")) {
            diverter_parser(&config->diverter, json);
        }
        if (!strcmp(json->string, "adguard")) {
            adguard_parser(&config->adguard, json);
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

void config_parser(cleardns_config *config, const char *config_file) {
    char *config_content = read_file(config_file);
    if (is_json_suffix(config_file)) { // JSON format
        log_info("Start JSON configure parser");
        cleardns_parser(config, config_content); // configure parser
    } else { // YAML or TOML format
        log_info("Start configure parser");
        char *json_content = to_json(config_content); // convert to json format
        if (json_content == NULL) {
            log_fatal("Configure parser error");
        }
        cleardns_parser(config, json_content); // configure parser
        free(json_content);
    }
    free(config_content);
    log_info("Configure parser success");
}
