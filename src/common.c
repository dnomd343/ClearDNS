#include "common.h"
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
