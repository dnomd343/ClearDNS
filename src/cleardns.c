#include "logger.h"
#include "common.h"
#include "dnsproxy.h"

#include "strList.h"

//#include <stdio.h>
//#include <string.h>
//#include "common.h"
//#include "process.h"

//char *init_script = "/usr/bin/load";
//char *custom_script = "/etc/cleardns/custom.sh";
//
//char *adguard_workdir = "/etc/cleardns/AdGuardHome";
//char *overture_config = "/etc/overture/config.yml";
//char *upstream_config = "/etc/cleardns/upstream.json";

//void show_command(char *title, char **command) {
//    int num = 0;
//    fprintf(stderr, "%s => \"", title);
//    while(command[num] != NULL) {
//        fprintf(stderr, "%s", command[num++]);
//        if (command[num] != NULL) {
//            fprintf(stderr, " ");
//        }
//    }
//    fprintf(stderr, "\"\n");
//}

int main(int argc, char *argv[]) { // ClearDNS server

    LOG_LEVEL = LOG_DEBUG;

    log_info("ClearDNS server start (%s)", VERSION);

    dnsproxy *domestic = dnsproxy_init(DOMESTIC_PORT);

    dnsproxy_add_bootstrap(domestic, "1.1.1.1");
    dnsproxy_add_bootstrap(domestic, "8.8.8.8");

    dnsproxy_add_primary(domestic, "223.5.5.5");
    dnsproxy_add_primary(domestic, "tls://dns.pub");

    dnsproxy_add_fallback(domestic, "tls://223.6.6.6");
    dnsproxy_add_fallback(domestic, "tls://120.53.53.53");

    domestic->verify = FALSE;
//    domestic->parallel = FALSE;
    domestic->optimistic = TRUE;
//    domestic->cache = "0";

    dnsproxy_dump("Domestic", domestic);

    dnsproxy_gen_config(domestic);

//    int debug_mode = 0;
//    fprintf(stderr, "[ClearDNS] Server start.\n");
//    for (char **p = argv; p < argv + argc; ++p) {
//        if (!strcmp(*p, "--debug")) { // --debug option
//            ++debug_mode;
//        }
//    }
//
//    init_server(init_script, custom_script); // run init script and custom script
//
//    load_start_command(adguard_workdir, overture_config, upstream_config, debug_mode); // generate commands
//    if (debug_mode) { // show exec command
//        fprintf(stderr, "[ClearDNS] Debug mode.\n");
//        show_command("[ClearDNS] adguard", adguard_command);
//        show_command("[ClearDNS] overture", overture_command);
//        show_command("[ClearDNS] dnsproxy (domestic)", domestic_dnsproxy_command);
//        show_command("[ClearDNS] dnsproxy (foreign)", foreign_dnsproxy_command);
//    }
//
//    server_daemon(); // run as daemon to manage process in docker
    return 0;
}
