#include <stdlib.h>
#include "config.h"
#include "common.h"
#include "logger.h"
#include "structure.h"

cleardns_config* config_init() { // init config struct of cleardns
    cleardns_config *config = (cleardns_config *)malloc(sizeof(cleardns_config));
    config->port = DNS_PORT;
    config->cache.size = 0; // disable cache in default
    config->cache.enable = FALSE;
    config->cache.optimistic = FALSE;

    config->domestic.port = DOMESTIC_PORT;
    config->domestic.verify = TRUE;
    config->domestic.parallel = TRUE;
    config->domestic.bootstrap = string_list_init();
    config->domestic.fallback = string_list_init();
    config->domestic.primary = string_list_init();

    config->foreign.port = FOREIGN_PORT;
    config->foreign.verify = TRUE;
    config->foreign.parallel = TRUE;
    config->foreign.bootstrap = string_list_init();
    config->foreign.fallback = string_list_init();
    config->foreign.primary = string_list_init();

    config->diverter.port = DIVERTER_PORT;
    config->diverter.gfwlist = string_list_init();
    config->diverter.china_ip = string_list_init();
    config->diverter.chinalist = string_list_init();

    config->adguard.port = ADGUARD_PORT;
    config->adguard.enable = TRUE;
    config->adguard.username = string_init(ADGUARD_USER);
    config->adguard.password = string_init(ADGUARD_PASSWD);

    config->reject = uint32_list_init();
    config->hosts = string_list_init();
    config->ttl = string_list_init();
    return config;
}

void config_dump(cleardns_config *config) { // dump config info of cleardns
    log_debug("DNS port -> %u", config->port);
    log_debug("Cache size -> %u", config->cache.size);
    log_debug("Cache enable -> %s", show_bool(config->cache.enable));
    log_debug("Cache optimistic -> %s", show_bool(config->cache.optimistic));

    log_debug("Domestic port -> %u", config->domestic.port);
    log_debug("Domestic verify -> %s", show_bool(config->domestic.verify));
    log_debug("Domestic parallel -> %s", show_bool(config->domestic.parallel));
    string_list_debug("Domestic bootstrap", config->domestic.bootstrap);
    string_list_debug("Domestic fallback", config->domestic.fallback);
    string_list_debug("Domestic primary", config->domestic.primary);

    log_debug("Foreign port -> %u", config->foreign.port);
    log_debug("Foreign verify -> %s", show_bool(config->foreign.verify));
    log_debug("Foreign parallel -> %s", show_bool(config->foreign.parallel));
    string_list_debug("Foreign bootstrap", config->foreign.bootstrap);
    string_list_debug("Foreign fallback", config->foreign.fallback);
    string_list_debug("Foreign primary", config->foreign.primary);

    log_debug("Diverter port -> %u", config->diverter.port);
    string_list_debug("Diverter gfwlist", config->diverter.gfwlist);
    string_list_debug("Diverter china-ip", config->diverter.china_ip);
    string_list_debug("Diverter chinalist", config->diverter.chinalist);

    log_debug("AdGuardHome port -> %u", config->adguard.port);
    log_debug("AdGuardHome enable -> %s", show_bool(config->adguard.enable));
    log_debug("AdGuardHome username -> %s", config->adguard.username);
    log_debug("AdGuardHome password -> %s", config->adguard.password);

    uint32_list_debug("DNS reject type", config->reject);
    string_list_debug("Domain TTL", config->ttl);
    string_list_debug("Hosts", config->hosts);
}

void config_free(cleardns_config *config) { // free config struct of cleardns
    string_list_free(config->domestic.bootstrap);
    string_list_free(config->domestic.fallback);
    string_list_free(config->domestic.primary);

    string_list_free(config->foreign.bootstrap);
    string_list_free(config->foreign.fallback);
    string_list_free(config->foreign.primary);

    string_list_free(config->diverter.gfwlist);
    string_list_free(config->diverter.china_ip);
    string_list_free(config->diverter.chinalist);

    uint32_list_free(config->reject);
    string_list_free(config->hosts);
    string_list_free(config->ttl);
    free(config->adguard.username);
    free(config->adguard.password);
    free(config);
}
