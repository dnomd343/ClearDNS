#include <stdlib.h>
#include "dnsproxy.h"
#include "logger.h"
#include "common.h"
#include "strList.h"

char* show_bool(int value) {
    if (value) {
        return "true";
    } else {
        return "false";
    }
}

char** command_init(char *file) {
    return string_list_append(string_list_init(), file);
}

dnsproxy* dnsproxy_init(int port) {
    dnsproxy *info = (dnsproxy*)malloc(sizeof(dnsproxy));
    info->port = port;
    info->verify = TRUE;
    info->parallel = TRUE;
    info->optimistic = FALSE;
    info->bootstrap = string_list_init();
    info->fallback = string_list_init();
    info->primary = string_list_init();
    info->cache = "4194304"; // 4MiB
    return info;
}

void dnsproxy_dump(char *caption, dnsproxy *info) {
    log_debug("%s listen port -> %d", caption, info->port);
    log_debug("%s verify -> %s", caption, show_bool(info->verify));
    log_debug("%s parallel -> %s", caption, show_bool(info->parallel));
    log_debug("%s optimistic -> %s", caption, show_bool(info->optimistic));

    log_debug("%s bootstrap -> %s", caption, string_list_dump(info->bootstrap));
    log_debug("%s fallback -> %s", caption, string_list_dump(info->fallback));
    log_debug("%s primary -> %s", caption, string_list_dump(info->primary));

    log_debug("%s cache -> %s", caption, info->cache);

    char **test = string_list_init();
    test = string_list_append(test, "0");
    test = string_list_append(test, "1");
    test = string_list_append(test, "2");

//    char *test[] = {NULL};
//    char *test[] = {"dnomd343", "2333", NULL};
    log_warn("%s", string_list_dump(test));
    log_warn("%d", string_list_len(test));

}
