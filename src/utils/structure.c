#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"

int* int_init(int number) {
    int *data = (int*)malloc(sizeof(int));
    *data = number;
    return data;
}

char* string_init(const char *str) {
    return strcpy((char*)malloc(strlen(str) + 1), str);
}

char* string_join(const char *base, const char *add) {
    char *ret = (char*)malloc(sizeof(char) * (strlen(base) + strlen(add) + 1));
    return strcat(strcpy(ret, base), add);
}

char** string_list_init() {
    char **string_list = (char**)malloc(sizeof(char*));
    *string_list = NULL;
    return string_list;
}

int string_list_len(char **string_list) {
    int num = 0;
    while(string_list[num++] != NULL); // get string list size
    return num - 1;
}

char** string_list_append(char **string_list, const char *string) {
    int len = string_list_len(string_list);
    string_list = (char**)realloc(string_list, sizeof(char*) * (len + 2));
    string_list[len] = string_init(string);
    string_list[len + 1] = NULL; // list end sign
    return string_list;
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

int** int_list_init() {
    int **int_list = (int**)malloc(sizeof(int*));
    *int_list = NULL;
    return int_list;
}

int int_list_len(int **int_list) {
    int num = 0;
    while(int_list[num++] != NULL); // get int list size
    return num - 1;
}

int** int_list_append(int **int_list, int number) {
    int len = int_list_len(int_list);
    int_list = (int**)realloc(int_list, sizeof(int*) * (len + 2));
    int_list[len] = int_init(number);
    int_list[len + 1] = NULL; // list end sign
    return int_list;
}

char* int_list_dump(int **int_list) { // [1, 2, 3, ...]
    if (int_list_len(int_list) == 0) {
        return string_init("[]"); // empty int list
    }
    char int_str[12];
    char *string_ret = (char*)malloc(sizeof(char) * 2);
    string_ret[0] = '['; string_ret[1] = 0x00;
    for (int **number = int_list; *number != NULL; ++number) {
        sprintf(int_str, "%d", **number);
        string_ret = (char*)realloc(string_ret, sizeof(char) * (strlen(string_ret) + 15));
        string_ret = strcat(strcat(string_ret, int_str), ", ");
    }
    string_ret[strlen(string_ret) - 2] = ']';
    string_ret[strlen(string_ret) - 1] = 0x00;
    return string_ret;
}
