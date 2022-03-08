#include <stdio.h>
#include <string.h>
#include "common.h"
#include "process.h"

int debug_mode = 0;

char *init_script = "/usr/bin/load";
char *custom_script = "/etc/cleardns/custom.sh";

char *adguard_workdir = "/etc/cleardns/AdGuardHome";
char *overture_config = "/etc/overture/config.yml";
char *upstream_config = "/etc/cleardns/upstream.json";

void show_command(char *title, char **command) {
    int num = 0;
    fprintf(stderr, "\n%s => ", title);
    while(command[num] != NULL) {
        fprintf(stderr, "%s ", command[num++]);
    }
    fprintf(stderr, "\n");
}

int main(int argc, char *argv[]) {
    for (char **p = argv; p < argv + argc; ++p) {
        if (!strcmp(*p, "--debug")) { // --debug option
            ++debug_mode;
        }
    }

    load_start_command(adguard_workdir, overture_config, upstream_config, debug_mode); // generate commands
    if (debug_mode) { // show exec command
        char *line = "----------------------------------------------------------------";
        fprintf(stderr, "\n%s%s\n", line, line);
        show_command("crond", crond_command);
        show_command("AdGuard", adguard_command);
        show_command("overture", overture_command);
        show_command("dnsproxy (domestic)", domestic_dnsproxy_command);
        show_command("dnsproxy (foreign)", foreign_dnsproxy_command);
        fprintf(stderr, "\n%s%s\n", line, line);
    }

    init_server(init_script, custom_script);

    return 0;
}
