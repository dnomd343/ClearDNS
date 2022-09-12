#include <stdio.h>
#include "common.h"
#include "logger.h"
#include "structure.h"

char* show_bool(int value) {
    if (value) {
        return "true";
    } else {
        return "false";
    }
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
