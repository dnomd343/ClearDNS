#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "logger.h"

char* show_bool(int value) { // return `true` or `false`
    if (value) {
        return "true";
    } else {
        return "false";
    }
}

void save_file(char *file, char *content) { // save into file
    log_debug("Write into `%s` -> \n%s", file, content);
    FILE* fp = fopen(file , "w");
    if (fp == NULL) {
        log_fatal("Fail to open file -> %s", file);
    }
    fputs(content, fp);
    fclose(fp);
    log_debug("Save `%s` success", file);
}

char* read_file(char *file) { // read file content
    log_debug("Read file -> %s", file);
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) { // file open failed
        log_fatal("File `%s` open failed", file);
    }
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp); // get file length
    char *content = (char*)malloc(length + 1); // malloc new memory
    if (content == NULL) {
        log_fatal("No enough memory for reading file"); // file too large
    }
    rewind(fp);
    fread(content, 1, length, fp); // read file stream
    content[length] = '\0'; // set end flag
    fclose(fp);
    log_debug("File `%s` read success ->\n%s", file, content);
    return content;
}
