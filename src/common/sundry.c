#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "logger.h"
#include "constant.h"
#include "structure.h"

char* show_bool(uint8_t value) { // return `true` or `false`
    if (value) {
        return "true";
    }
    return "false";
}

char* string_join(const char *base, const char *add) { // combine string
    char *ret = (char *)malloc(strlen(base) + strlen(add) + 1);
    return strcat(strcpy(ret, base), add);
}

char* string_load(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *buf = NULL;
    vasprintf(&buf, fmt, ap);
    return buf;
}

char* uint32_to_string(uint32_t number) { // convert uint32 -> string
    char to_str[11]; // MAX_LEN(uint32) -> 4294967296(10-bytes)
    sprintf(to_str, "%u", number);
    return strdup(to_str);
}

void string_list_debug(char *describe, char **string_list) { // show string list in debug log
    char *string_ret = string_list_dump(string_list);
    log_debug("%s -> %s", describe, string_ret);
    free(string_ret);
}

void uint32_list_debug(char *describe, uint32_t **uint32_list) { // show uint32 list in debug log
    char *string_ret = uint32_list_dump(uint32_list);
    log_debug("%s -> %s", describe, string_ret);
    free(string_ret);
}

uint8_t check_port(uint16_t port) { // whether port is valid
    if (port > 0 && port <= 65535) { // 1 ~ 65535
        return TRUE;
    }
    return FALSE;
}

uint16_t gen_rand_num(uint16_t limit) { // 0 ~ (limit - 1)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
    usleep(1); // sleep 1us (different time seed)
    return rand() % limit; // NOLINT
}
