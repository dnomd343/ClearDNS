#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"

char* string_init(const char *str) { // new string
    return strcpy((char*)malloc(strlen(str) + 1), str);
}

char* string_join(const char *base, const char *add) { // combine string
    char *ret = (char*)malloc(sizeof(char) * (strlen(base) + strlen(add) + 1));
    return strcat(strcpy(ret, base), add);
}

char** string_list_init() { // init string list
    char **string_list = (char**)malloc(sizeof(char*));
    *string_list = NULL; // list end sign
    return string_list;
}

uint32_t string_list_len(char **string_list) { // get len of string list
    uint32_t num = 0;
    while(string_list[num++] != NULL); // get string list size
    return num - 1;
}

char** string_list_append(char **string_list, const char *string) { // add new string at the end of list
    uint32_t len = string_list_len(string_list);
    string_list = (char **)realloc(string_list, sizeof(char *) * (len + 2)); // extend string list
    string_list[len] = string_init(string);
    string_list[len + 1] = NULL; // list end sign
    return string_list;
}

// TODO: string list free
void string_list_free(char **string_list) { // free string list
    // free every string
    // free list itself
}

char* string_list_dump(char **string_list) { // ['a', 'b', 'c', ...]
    if (string_list_len(string_list) == 0) {
        return string_init("[]"); // empty string list
    }
    char *string_ret = (char *)malloc(2);
    strcpy(string_ret, "[");
    for (char **string = string_list; *string != NULL; ++string) {
        string_ret = (char *)realloc(string_ret, strlen(string_ret) + strlen(*string) + 5);
        strcat(strcat(strcat(string_ret, "'"), *string), "', ");
    }
    string_ret[strlen(string_ret) - 2] = '\0';
    return strcat(string_ret, "]");
}

uint32_t** uint32_list_init() {
    uint32_t **int_list = (uint32_t **)malloc(sizeof(uint32_t *));
    *int_list = NULL;
    return int_list;
}

uint32_t* uint32_init(uint32_t number) {
    uint32_t *data = (uint32_t *)malloc(sizeof(uint32_t));
    *data = number;
    return data;
}

uint32_t uint32_list_len(uint32_t **int_list) {
    uint32_t num = 0;
    while(int_list[num++] != NULL); // get int list size
    return num - 1;
}

uint32_t** uint32_list_append(uint32_t **uint32_list, uint32_t number) {
    uint32_t len = uint32_list_len(uint32_list);
    uint32_list = (uint32_t **)realloc(uint32_list, sizeof(uint32_t *) * (len + 2));
    uint32_list[len] = uint32_init(number);
    uint32_list[len + 1] = NULL; // list end sign
    return uint32_list;
}

char* uint32_list_dump(uint32_t **uint32_list) { // [1, 2, 3, ...]
    if (uint32_list_len(uint32_list) == 0) {
        return string_init("[]"); // empty int list
    }
    char uint32_str[12];
    char *string_ret = (char *)malloc(2);
    string_ret[0] = '[';
    string_ret[1] = '\0';
    for (uint32_t **number = uint32_list; *number != NULL; ++number) {
        sprintf(uint32_str, "%d", **number);
        string_ret = (char*)realloc(string_ret, strlen(string_ret) + 15);
        string_ret = strcat(strcat(string_ret, uint32_str), ", ");
    }
    string_ret[strlen(string_ret) - 2] = ']';
    string_ret[strlen(string_ret) - 1] = '\0';
    return string_ret;
}

// TODO: uint32 list free
