#include <stdio.h>
#include "common.h"
#include "logger.h"
#include "strList.h"

char* show_bool(int value) {
    if (value) {
        return "true";
    } else {
        return "false";
    }
}

char** command_init(char *bin) {
    return string_list_append(string_list_init(), bin);
}

void save_file(char *file, char *content) {
    log_debug("Write into `%s` -> \n%s", file, content);
    FILE* fp = fopen(file , "w");
    if (fp == NULL) {
        log_fatal("Fail to open file -> %s", file);
    }
    fputs(content, fp);
    fclose(fp);
    log_debug("Save `%s` success", file);
}
