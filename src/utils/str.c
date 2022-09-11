#include <stdlib.h>
#include <string.h>
#include "str.h"

char* string_init(char *str) {
    return strcpy((char*)malloc(strlen(str) + 1), str);
}

char* string_join(char *base, const char *add, int is_new) {
    unsigned long string_len = strlen(base) + strlen(add);
    if (is_new) {
        char *ret = (char*)malloc(sizeof(char) * (string_len + 1));
        return strcat(strcpy(ret, base), add);
    } else {
        base = (char*)realloc(base, sizeof(char) * (string_len + 1));
        return strcat(base, add);
    }
}

char* string_list_dump(char **string_list) { // ['a', 'b', 'c', ...]
    unsigned long string_len = 0;
    for (char **string = string_list; *string != NULL; ++string) {
        string_len += strlen(*string) + 4;
    }
    if (string_len == 0) { // empty string
        string_len = 2;
    }
    char *string_ret = (char*)malloc(sizeof(char) * (string_len + 1));
    string_ret[0] = '[';
    string_ret[1] = 0x00;
    for (char **string = string_list; *string != NULL; ++string) {
        string_ret = strcat(strcat(string_ret, "'"), *string);
        string_ret = strcat(string_ret, "', ");
    }
    string_ret[string_len - 1] = ']';
    string_ret[string_len] = 0x00;
    return string_ret;
}

int string_list_len(char **string_list) {
    int num = 0;
    while(string_list[num++] != NULL); // get string list size
    return num - 1;
}

char** string_list_init() {
    char **string_list = (char**)malloc(sizeof(char*));
    *string_list = NULL;
    return string_list;
}

char** string_list_append(char **string_list, char *string) {
    int len = string_list_len(string_list);
    string_list = (char**)realloc(string_list, sizeof(char*) * (len + 2));
    string_list[len] = string_init(string);
    string_list[len + 1] = NULL; // list end sign
    return string_list;
}
