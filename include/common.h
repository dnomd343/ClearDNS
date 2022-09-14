#ifndef _COMMON_H_
#define _COMMON_H_

//#include <stdlib.h>
//
//typedef u_int8_t uint8_t;
//typedef u_int16_t uint16_t;
//typedef u_int32_t uint32_t;
//typedef u_int64_t uint64_t;

#include <stdint.h>

#define VERSION "1.3.0-dev"

#define TRUE  1
#define FALSE 0

#define DNS_PORT      53
#define ADGUARD_PORT  80
#define DIVERTER_PORT 5353
#define DOMESTIC_PORT 4053
#define FOREIGN_PORT  6053

#define DIVERTER_TIMEOUT 6

#define ADGUARD_USER   "admin"
#define ADGUARD_PASSWD "adguard"

#define DNSPROXY_BIN "dnsproxy"
#define OVERTURE_BIN "overture"
#define ADGUARD_BIN  "AdGuardHome"

#define WORK_DIR "/etc/cleardns/"

//extern char **adguard_command;
//extern char **overture_command;
//extern char **domestic_dnsproxy_command;
//extern char **foreign_dnsproxy_command;

//void load_start_command(char *adguard_workdir, char *overture_config, char *upstream_config, int is_debug);

char* show_bool(uint8_t value);
char* read_file(const char *file);
void save_file(const char *file, const char *content);
void string_list_debug(char *describe, char **string_list);
void uint32_list_debug(char *describe, uint32_t **uint32_list);

uint8_t check_port(uint16_t port);
uint16_t gen_rand_num(uint16_t limit);
char* uint32_to_string(uint32_t number);

char* gen_bcrypt(const char *data);
int run_command(const char *command);
void create_folder(const char *folder);

char* string_init(const char *str);
char* string_join(const char *base, const char *add);

#include "cJSON.h"

char* to_json(const char *config_file);
cJSON* json_field_get(cJSON *entry, const char *field);
void json_field_replace(cJSON *entry, const char *field, cJSON *content);

uint8_t json_bool_value(char *key, cJSON *json);
uint32_t** json_uint32_list_value(char *key, cJSON *json, uint32_t **uint32_list);
char** json_string_list_value(char *key, cJSON *json, char **string_list);
char* json_string_value(char* key, cJSON *json);
int json_int_value(char *key, cJSON *json);
uint8_t is_json_suffix(const char *file_name);

#endif
