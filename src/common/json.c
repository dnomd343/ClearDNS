#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "logger.h"
#include "common.h"
#include "system.h"
#include "structure.h"

char* to_json(const char *file) {
    char flag[9];
    for (int i = 0; i < 8; ++i) { // generate 8-bits flag
        flag[i] = (char)gen_rand_num(10);
        flag[i] += 48;
    }
    flag[8] = '\0';
    char *output_file = string_join("/tmp/tojson-", flag);
    char *to_json_cmd = (char *)malloc(strlen(file) + strlen(output_file) + 11);
    sprintf(to_json_cmd, "toJSON %s > %s", file, output_file);
//    log_debug("JSON format command -> `%s`", to_json_cmd);
    if (run_command(to_json_cmd)) { // toJSON return non-zero code
        // TODO: try remove output file
        return NULL; // convert failed
    }
    free(to_json_cmd);

    char *json_content = read_file(output_file);
    char *rm_cmd = string_join("rm -f ", output_file);
    run_command(rm_cmd);
    free(output_file);
    free(rm_cmd);
    return json_content;
}

void json_field_replace(cJSON *entry, const char *field, cJSON *content) {
    if (!cJSON_ReplaceItemInObject(entry, field, content)) { // field not exist
        cJSON_AddItemToObject(entry, field, content); // add new field
    }
}

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
    return string_init(json->valuestring);
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

cJSON* json_field_get(cJSON *entry, const char *field) {
    cJSON *sub = entry->child;
    while (sub != NULL) {
        if (!strcmp(sub->string, field)) {
            return sub;
        }
        sub = sub->next; // next field
    }
    cJSON *new = cJSON_CreateObject();
    cJSON_AddItemToObject(entry, field, new);
    return new;
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
