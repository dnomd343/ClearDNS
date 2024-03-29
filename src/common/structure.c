#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"

char** string_list_init() { // init string list
    char **string_list = (char **)malloc(sizeof(char *));
    *string_list = NULL; // list end sign
    return string_list;
}

uint32_t string_list_len(char **string_list) { // get len of string list
    uint32_t num = 0;
    while(string_list[num++] != NULL); // get list size
    return num - 1;
}

void string_list_append(char ***string_list, const char *string) {
    uint32_t len = string_list_len(*string_list);
    *string_list = (char **)realloc(*string_list, sizeof(char *) * (len + 2)); // extend string list
    (*string_list)[len] = strdup(string);
    (*string_list)[len + 1] = NULL; // list end sign
}

void string_list_update(char ***base_list, char **update_list) { // combine two string list
    for (char **string = update_list; *string != NULL; ++string) {
        string_list_append(base_list, *string);
    }
}

void string_list_free(char **string_list) { // free string list
    for (char **string = string_list; *string != NULL; ++string) {
        free(*string);
    }
    free(string_list);
}

char* string_list_dump(char **string_list) { // ['a', 'b', 'c', ...]
    if (string_list_len(string_list) == 0) {
        return strdup("[]"); // empty string list
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

uint32_t** uint32_list_init() { // init uint32 list
    uint32_t **uint32_list = (uint32_t **)malloc(sizeof(uint32_t *));
    *uint32_list = NULL;
    return uint32_list;
}

uint32_t uint32_list_len(uint32_t **uint32_list) { // get len of uint32 list
    uint32_t num = 0;
    while(uint32_list[num++] != NULL); // get list size
    return num - 1;
}

void uint32_list_append(uint32_t ***uint32_list, uint32_t number) { // add new uint32 at the end of list
    uint32_t len = uint32_list_len(*uint32_list);
    *uint32_list = (uint32_t **)realloc(*uint32_list, sizeof(uint32_t *) * (len + 2));
    (*uint32_list)[len] = (uint32_t *)malloc(sizeof(uint32_t));
    *(*uint32_list)[len] = number;
    (*uint32_list)[len + 1] = NULL; // list end sign
}

void uint32_list_update(uint32_t ***base_list, uint32_t **update_list) { // combine two uint32 list
    for (uint32_t **number = update_list; *number != NULL; ++number) {
        uint32_list_append(base_list, **number);
    }
}

void uint32_list_free(uint32_t **uint32_list) { // free uint32 list
    for (uint32_t **number = uint32_list; *number != NULL; ++number) {
        free(*number);
    }
    free(uint32_list);
}

char* uint32_list_dump(uint32_t **uint32_list) { // [1, 2, 3, ...]
    if (uint32_list_len(uint32_list) == 0) {
        return strdup("[]"); // empty int list
    }
    char uint32_str[12];
    char *string_ret = (char *)malloc(2);
    strcpy(string_ret, "[");
    for (uint32_t **number = uint32_list; *number != NULL; ++number) {
        sprintf(uint32_str, "%u", **number);
        string_ret = (char *)realloc(string_ret, strlen(string_ret) + 15);
        string_ret = strcat(strcat(string_ret, uint32_str), ", ");
    }
    string_ret[strlen(string_ret) - 2] = '\0';
    return strcat(string_ret, "]");
}
