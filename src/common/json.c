#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "logger.h"
#include "sundry.h"
#include "to_json.h"
#include "constant.h"
#include "structure.h"

uint8_t is_json_suffix(const char *file_name) { // whether file name end with `.json` suffix
    if (strlen(file_name) <= 5) { // file name shorter than `.json`
        return FALSE;
    }
    if (!strcmp(file_name + strlen(file_name) - 5, ".json")) { // xxx.json
        return TRUE;
    }
    return FALSE;
}

char* to_json(const char *content) { // convert JSON / TOML / YAML to json format (failed -> NULL)
    const char *json_string = to_json_ffi(content); // convert to json format
    char *json_content = strdup(json_string); // load string into owner heap
    free_rust_string(json_string); // free rust string
    if (strlen(json_content) == 0) { // empty string -> convert error
        log_warn("JSON convert error ->\n%s", content);
        free(json_content);
        return NULL;
    }
    log_debug("JSON convert result ->\n%s", json_content);
    return json_content;
}


cJSON* json_field_get(cJSON *entry, const char *key) { // fetch key from json map (create when key not exist)
    cJSON *sub = entry->child;
    while (sub != NULL) { // traverse all keys
        if (!strcmp(sub->string, key)) { // target key found
            return sub;
        }
        sub = sub->next;
    }
    cJSON *new = cJSON_CreateObject(); // create new json key
    cJSON_AddItemToObject(entry, key, new);
    return new;
}

void json_field_replace(cJSON *entry, const char *key, cJSON *content) {
    if (!cJSON_ReplaceItemInObject(entry, key, content)) { // key not exist
        cJSON_AddItemToObject(entry, key, content); // add new json key
    }
}

int json_int_value(char *caption, cJSON *json) { // json int or string value -> int
    if (cJSON_IsNumber(json)) {
        return json->valueint;
    } else if (cJSON_IsString(json)) {
        char *p;
        int int_ret = (int)strtol(json->valuestring, &p, 10);
        if (int_ret == 0 && strcmp(json->valuestring, "0") != 0) { // invalid number in string
            log_fatal("`%s` not a valid number", caption);
        }
        return int_ret;
    } else {
        log_fatal("`%s` must be number or string", caption);
    }
    return 0; // never reach
}

uint8_t json_bool_value(char *caption, cJSON *json) { // json bool value -> bool
    if (!cJSON_IsBool(json)) {
        log_fatal("`%s` must be boolean", caption);
    }
    return json->valueint;
}

char* json_string_value(char* caption, cJSON *json) { // json string value -> string
    if (!cJSON_IsString(json)) {
        log_fatal("`%s` must be string", caption);
    }
    return strdup(json->valuestring);
}

char** json_string_list_value(char *caption, cJSON *json, char **string_list) { // json string array
    if (cJSON_IsString(json)) {
        string_list_append(&string_list, json->valuestring);
    } else if (cJSON_IsArray(json)) {
        json = json->child;
        while (json != NULL) {
            if (!cJSON_IsString(json)) {
                log_fatal("`%s` must be string array", caption);
            }
            string_list_append(&string_list, json->valuestring);
            json = json->next; // next key
        }
    } else if (!cJSON_IsNull(json)) { // allow null -> empty string list
        log_fatal("`%s` must be array or string", caption);
    }
    return string_list;
}

uint32_t** json_uint32_list_value(char *caption, cJSON *json, uint32_t **uint32_list) { // json uint32 array
    if (cJSON_IsNumber(json)) {
        uint32_list_append(&uint32_list, json->valueint);
    } else if (cJSON_IsArray(json)) {
        json = json->child;
        while (json != NULL) {
            if (!cJSON_IsNumber(json)) {
                log_fatal("`%s` must be number array", caption);
            }
            uint32_list_append(&uint32_list, json->valueint);
            json = json->next; // next key
        }
    } else if (!cJSON_IsNull(json)) { // allow null -> empty uint32 list
        log_fatal("`%s` must be array or number", caption);
    }
    return uint32_list;
}

void json_string_map_value(char *caption, cJSON *json, char ***key_list, char ***value_list) { // json string map
    if (!cJSON_IsObject(json)) {
        log_fatal("`%s` must be map", caption);
    }
    json = json->child;
    while (json != NULL) { // traverse all json field
        if (!cJSON_IsString(json)) {
            log_fatal("`%s` must be string-string map", caption);
        }
        string_list_append(key_list, json->string);
        string_list_append(value_list, json->valuestring);
        json = json->next;
    }
}
