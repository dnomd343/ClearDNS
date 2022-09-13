#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toJSON.h"
#include "logger.h"
#include "common.h"

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
    log_debug("JSON format command -> `%s`", to_json_cmd);
    if (run_command(to_json_cmd)) { // toJSON return non-zero code
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
