#include <stdlib.h>
#include "loader.h"
#include "config.h"
#include "parser.h"
#include "dnsproxy.h"
#include "structure.h"
#include "common.h"

struct cleardns loader;

dnsproxy* load_domestic(cleardns_config *config) {
    dnsproxy *domestic = dnsproxy_init(config->domestic.port);
    domestic->verify = config->domestic.verify;
    domestic->parallel = config->domestic.parallel;
    if (config->cache.enable) {
        domestic->cache = config->cache.size;
        domestic->optimistic = config->cache.optimistic;
    }
    domestic->bootstrap = string_list_update(domestic->bootstrap, config->domestic.bootstrap);
    domestic->fallback = string_list_update(domestic->fallback, config->domestic.fallback);
    domestic->primary = string_list_update(domestic->primary, config->domestic.primary);
    return domestic;
}

dnsproxy* load_foreign(cleardns_config *config) {
    dnsproxy *foreign = dnsproxy_init(config->foreign.port);
    foreign->verify = config->foreign.verify;
    foreign->parallel = config->foreign.parallel;
    if (config->cache.enable) {
        foreign->cache = config->cache.size;
        foreign->optimistic = config->cache.optimistic;
    }
    foreign->bootstrap = string_list_update(foreign->bootstrap, config->foreign.bootstrap);
    foreign->fallback = string_list_update(foreign->fallback, config->foreign.fallback);
    foreign->primary = string_list_update(foreign->primary, config->foreign.primary);
    return foreign;
}

overture* load_diverter(cleardns_config *config) {
    overture *diverter = overture_init();
    diverter->port = config->diverter.port;
    diverter->foreign_port = config->foreign.port;
    diverter->domestic_port = config->domestic.port;

    free(diverter->ttl_file);
    free(diverter->host_file);
    // TODO: load into ASSET_TTL
    // TODO: load into ASSET_HOSTS
    diverter->ttl_file = string_init(ASSET_TTL);
    diverter->host_file = string_init(ASSET_HOSTS);

    free(diverter->domestic_ip_file);
    free(diverter->foreign_domain_file);
    free(diverter->domestic_domain_file);
    // TODO: load into ASSET_CHINA_IP
    // TODO: load into ASSET_GFW_LIST
    // TODO: load into ASSET_CHINA_LIST
    diverter->domestic_ip_file = string_init(ASSET_CHINA_IP);
    diverter->foreign_domain_file = string_init(ASSET_GFW_LIST);
    diverter->domestic_domain_file = string_init(ASSET_CHINA_LIST);

    // TODO: assets file append

    diverter->reject_type = uint32_list_update(diverter->reject_type, config->reject);
    return diverter;
}

adguard* load_filter(cleardns_config *config) {
    if (!config->adguard.enable) {
        return NULL; // disable adguard
    }
    adguard *filter = adguard_init();
    filter->dns_port = config->port;
    filter->web_port = config->adguard.port;
    filter->username = config->adguard.username;
    filter->password = config->adguard.password;
    char *diverter_port = uint32_to_string(config->diverter.port);
    filter->upstream = string_join("127.0.0.1:", diverter_port);
    free(diverter_port);
    return filter;
}

void load_config(const char *config_file) {
    cleardns_config *config = config_init();
    config_parser(config, config_file);
    config_dump(config);
    if (!config->adguard.enable) {
        config->diverter.port = config->port; // override diverter port by dns port
    }
    loader.domestic = load_domestic(config);
    loader.foreign = load_foreign(config);
    loader.diverter = load_diverter(config);
    loader.filter = load_filter(config);
    config_free(config);
}
