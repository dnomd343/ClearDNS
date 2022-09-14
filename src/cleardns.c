#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "loader.h"
#include "logger.h"
#include "common.h"
#include "dnsproxy.h"
#include "overture.h"
#include "structure.h"
#include "adguard.h"

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


//    char **temp = string_list_init();
//
//    temp = string_list_append(temp, "a");
//    temp = string_list_append(temp, "b");
//    temp = string_list_append(temp, "c");
//
//    save_string_list("test.txt", temp);


    load_config("test.json");

    dnsproxy_load("Domestic", loader.domestic, "domestic.json");
    dnsproxy_load("Foreign", loader.foreign, "foreign.json");
    overture_load(loader.diverter, "overture.json");
    adguard_load(loader.filter, ADGUARD_DIR);


//    char **temp = string_list_init();
//    temp = string_list_append(temp, "123");
//    temp = string_list_append(temp, "abc");
//    temp = string_list_append(temp, "ok");
//
//    char *str = string_list_dump(temp);
//    log_info("`%s`", str);
//    free(str);
//    string_list_free(temp);

//    uint32_t **temp = uint32_list_init();
//    temp = uint32_list_append(temp, 123);
//    temp = uint32_list_append(temp, 456);
//    temp = uint32_list_append(temp, 789);
//
//    char *str = uint32_list_dump(temp);
//    log_info("`%s`\n", str);
//    free(str);
//    uint32_list_free(temp);


//    dnsproxy *domestic = dnsproxy_init(DOMESTIC_PORT);
//
//    dnsproxy_add_bootstrap(domestic, "1.1.1.1");
//    dnsproxy_add_bootstrap(domestic, "8.8.8.8");
//
//    dnsproxy_add_primary(domestic, "223.5.5.5");
//    dnsproxy_add_primary(domestic, "tls://dns.pub");
//
//    dnsproxy_add_fallback(domestic, "tls://223.6.6.6");
//    dnsproxy_add_fallback(domestic, "tls://120.53.53.53");
//
//    domestic->verify = FALSE;
//    domestic->parallel = FALSE;
//    domestic->optimistic = TRUE;
//    domestic->debug = TRUE;
//    domestic->cache = 4194304; // 4MiB
//
//    process *p = dnsproxy_load("Domestic", domestic, "domestic.json");
//    log_info("cmd -> %s", string_list_dump(p->cmd));
//    log_info("env -> %s", string_list_dump(p->env));
//    log_info("cwd -> %s", p->cwd);


//    overture *diverter = overture_init();
//
//    diverter->port = 5454;
//    diverter->timeout = 8;
//    diverter->domestic_ip_file = "china-ip.txt";
//    diverter->domestic_domain_file = "chinalist.txt";
//    diverter->foreign_domain_file = "gfwlist.txt";
//
//    diverter->debug = TRUE;
//    diverter->ttl_file = "domain_ttl.txt";
//    diverter->host_file = "hosts.txt";
//    diverter->reject_type = uint32_list_append(diverter->reject_type, 255);
//    diverter->reject_type = uint32_list_append(diverter->reject_type, 254);
//
//    process *p = overture_load(diverter, "overture.json");
//    log_info("cmd -> %s", string_list_dump(p->cmd));
//    log_info("env -> %s", string_list_dump(p->env));
//    log_info("cwd -> %s", p->cwd);


//    adguard *filter = adguard_init();
//
//    filter->debug = TRUE;
//    filter->dns_port = 54;
//    filter->web_port = 8080;
//    filter->upstream = "127.0.0.1:5454";
//
//    filter->username = "dnomd343";
//    filter->password = "password";
//
//    process *p = adguard_load(filter, "/cleardns/adguard/");
//    log_info("cmd -> %s", string_list_dump(p->cmd));
//    log_info("env -> %s", string_list_dump(p->env));
//    log_info("cwd -> %s", p->cwd);


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
