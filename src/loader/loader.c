#include "loader.h"
#include "config.h"
#include "parser.h"
#include "dnsproxy.h"
#include "structure.h"

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
    overture *diverter = overture_init(config->diverter.port);

    // timeout
    // ttl_file
    // host_file
    // foreign_port
    // domestic_port

    // reject_type

    // foreign_ip_file
    // domestic_ip_file
    // foreign_domain_file
    // domestic_domain_file
    return diverter;
}

void load_config(const char *config_file) {
    cleardns_config *config = config_init();
    config_parser(config, config_file);
    config_dump(config);

    // TODO: use dns port as diverter when adguard disabled

    loader.domestic = load_domestic(config);
    loader.foreign = load_foreign(config);
    loader.diverter = load_diverter(config);
    // load adguard
    config_free(config);
}
