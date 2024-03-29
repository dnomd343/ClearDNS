#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "logger.h"
#include "sundry.h"
#include "constant.h"
#include "structure.h"

cleardns_config* config_init() { // init config struct of cleardns
    cleardns_config *config = (cleardns_config *)malloc(sizeof(cleardns_config));
    config->port = DNS_PORT;
    config->cache.size = 0; // disable cache in default
    config->cache.enable = FALSE;
    config->cache.optimistic = FALSE;

    config->domestic.port = DOMESTIC_PORT;
    config->domestic.ipv6 = TRUE;
    config->domestic.verify = TRUE;
    config->domestic.parallel = TRUE;
    config->domestic.bootstrap = string_list_init();
    config->domestic.fallback = string_list_init();
    config->domestic.primary = string_list_init();

    config->foreign.port = FOREIGN_PORT;
    config->foreign.ipv6 = TRUE;
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
    config->adguard.username = strdup(ADGUARD_USER);
    config->adguard.password = strdup(ADGUARD_PASSWD);

    config->assets.disable = FALSE;
    config->assets.cron = strdup(UPDATE_CRON);
    config->assets.resources = assets_init();

    config->reject = uint32_list_init();
    config->hosts = string_list_init();
    config->ttl = string_list_init();
    config->script = string_list_init();
    return config;
}

void config_dump(cleardns_config *config) { // dump config info of cleardns
    log_debug("DNS port -> %u", config->port);
    log_debug("Cache size -> %u", config->cache.size);
    log_debug("Cache enable -> %s", show_bool(config->cache.enable));
    log_debug("Cache optimistic -> %s", show_bool(config->cache.optimistic));

    log_debug("Domestic port -> %u", config->domestic.port);
    log_debug("Domestic ipv6 -> %s", show_bool(config->domestic.ipv6));
    log_debug("Domestic verify -> %s", show_bool(config->domestic.verify));
    log_debug("Domestic parallel -> %s", show_bool(config->domestic.parallel));
    string_list_debug("Domestic bootstrap", config->domestic.bootstrap);
    string_list_debug("Domestic fallback", config->domestic.fallback);
    string_list_debug("Domestic primary", config->domestic.primary);

    log_debug("Foreign port -> %u", config->foreign.port);
    log_debug("Foreign ipv6 -> %s", show_bool(config->foreign.ipv6));
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

    log_debug("Assets disable -> %s", show_bool(config->assets.disable));
    log_debug("Assets update cron -> `%s`", config->assets.cron);
    log_debug("Assets with %d resource items", assets_size(config->assets.resources));
    assets_dump(config->assets.resources);

    uint32_list_debug("DNS reject type", config->reject);
    string_list_debug("Domain TTL", config->ttl);
    string_list_debug("Hosts", config->hosts);
    string_list_debug("Custom script", config->script);
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

    free(config->adguard.username);
    free(config->adguard.password);

    free(config->assets.cron);
    assets_free(config->assets.resources);

    uint32_list_free(config->reject);
    string_list_free(config->hosts);
    string_list_free(config->ttl);
    string_list_free(config->script);
    free(config);
}
