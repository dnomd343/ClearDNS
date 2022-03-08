#include <stdio.h>
#include <string.h>
#include "common.h"

int debug_mode = 0;

char *adguard_workdir = "/etc/cleardns/AdGuardHome";
char *overture_config = "/etc/overture/config.yml";
char *upstream_config = "/etc/cleardns/upstream.json";

void show_command(char **command) {
    int num = 0;
    while(command[num] != NULL) {
        printf("%s\n", command[num++]);
    }
}

int main(int argc, char *argv[]) {
    for (char **p = argv; p < argv + argc; ++p) {
        if (!strcmp(*p, "--debug")) { // --debug option
            ++debug_mode;
        }
    }

    load_start_command(adguard_workdir, overture_config, upstream_config, debug_mode);

    printf("[init]\n");
    show_command(init_command);
    printf("\n");

    printf("[crond]\n");
    show_command(crond_command);
    printf("\n");

    printf("[AdGuardHome]\n");
    show_command(adguard_command);
    printf("\n");

    printf("[overture]\n");
    show_command(overture_command);
    printf("\n");

    printf("[domestic dnsproxy]\n");
    show_command(domestic_dnsproxy_command);
    printf("\n");

    printf("[foreign dnsproxy]\n");
    show_command(foreign_dnsproxy_command);
    printf("\n");

    printf("done.\n");
    return 0;
}

