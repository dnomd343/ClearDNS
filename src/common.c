#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "common.h"
#include "logger.h"
#include "structure.h"

char* show_bool(uint8_t value) { // return `true` or `false`
    if (value) {
        return "true";
    } else {
        return "false";
    }
}

char* string_init(const char *str) { // new string
    return strcpy((char *)malloc(strlen(str) + 1), str);
}

char* string_join(const char *base, const char *add) { // combine string
    char *ret = (char *)malloc(strlen(base) + strlen(add) + 1);
    return strcat(strcpy(ret, base), add);
}

void string_list_debug(char *describe, char **string_list) {
    char *string_ret = string_list_dump(string_list);
    log_debug("%s -> %s", describe, string_ret);
    free(string_ret);
}

void uint32_list_debug(char *describe, uint32_t **uint32_list) {
    char *string_ret = uint32_list_dump(uint32_list);
    log_debug("%s -> %s", describe, string_ret);
    free(string_ret);
}

uint8_t check_port(uint16_t port) {
    if (port > 0 && port <= 65535) { // 1 ~ 65535
        return TRUE;
    }
    return FALSE;
}

char* uint32_to_string(uint32_t number) {
    char to_str[11]; // 32-bits (MAX_LEN -> 4294967296 -> 10-bytes)
    sprintf(to_str, "%u", number);
    return string_init(to_str);
}

uint16_t gen_rand_num(uint16_t limit) { // 0 ~ (limit - 1)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
    return rand() % limit; // NOLINT
}

int run_command(const char *command) {
    return system(command) / 256;
}

void save_file(const char *file, const char *content) { // save into file
    log_debug("Write into `%s` -> \n%s", file, content);
    FILE* fp = fopen(file , "w");
    if (fp == NULL) {
        log_fatal("Fail to open file -> %s", file);
    }
    fputs(content, fp);
    fclose(fp);
    log_debug("Save `%s` success", file);
}

char* read_file(const char *file) { // read file content
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
