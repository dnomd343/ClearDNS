#include <string.h>
#include "load.h"
#include "cJSON.h"
#include "common.h"
#include "logger.h"

void json_config_parser(cleardns_config *config, const char *config_file) { // JSON format configure
    char *config_content = read_file(config_file);
    cJSON *json = cJSON_Parse(config_content);
    if (json == NULL) {
        log_fatal("JSON format error");
    }
    json = json->child;
    while (json != NULL) {
        if (!strcmp(json->string, "port")) {
            // get dns port
        }

        json = json->next; // next field
    }
    cJSON_free(json); // free JSON struct
}
