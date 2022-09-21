#ifndef JSON_H_
#define JSON_H_

#include <stdint.h>
#include "cJSON.h"

char* to_json(const char *config_file);
uint8_t is_json_suffix(const char *file_name);
cJSON* json_field_get(cJSON *entry, const char *key);
void json_field_replace(cJSON *entry, const char *key, cJSON *content);

int json_int_value(char *caption, cJSON *json);
uint8_t json_bool_value(char *caption, cJSON *json);
char* json_string_value(char* caption, cJSON *json);
char** json_string_list_value(char *caption, cJSON *json, char **string_list);
uint32_t** json_uint32_list_value(char *caption, cJSON *json, uint32_t **uint32_list);
void json_string_map_value(char *caption, cJSON *json, char ***key_list, char ***value_list);

#endif
