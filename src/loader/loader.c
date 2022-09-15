#include <stdlib.h>
#include "config.h"
#include "loader.h"
#include "parser.h"
#include "sundry.h"
#include "system.h"
#include "dnsproxy.h"
#include "constant.h"
#include "structure.h"

struct cleardns loader;

void load_diverter_assets() {
    // ${ASSETS_DIR}${ASSET_GFW_LIST} >> ${WORK_DIR}${ASSET_GFW_LIST}
    // ${ASSETS_DIR}${ASSET_CHINA_IP} >> ${WORK_DIR}${ASSET_CHINA_IP}
    // ${ASSETS_DIR}${ASSET_CHINA_LIST} >> ${WORK_DIR}${ASSET_CHINA_LIST}
}

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

    if (string_list_len(config->ttl)) {
        free(diverter->ttl_file);
        diverter->ttl_file = string_init(ASSET_TTL);
        char *ttl_file = string_join(WORK_DIR, ASSET_TTL);
        save_string_list(ttl_file, config->ttl);
        free(ttl_file);
    }
    if (string_list_len(config->hosts)) {
        free(diverter->host_file);
        diverter->host_file = string_init(ASSET_HOSTS);
        char *hosts_file = string_join(WORK_DIR, ASSET_HOSTS);
        save_string_list(hosts_file, config->hosts);
        free(hosts_file);
    }

    free(diverter->domestic_ip_file);
    free(diverter->foreign_domain_file);
    free(diverter->domestic_domain_file);
    diverter->domestic_ip_file = string_init(ASSET_CHINA_IP);
    diverter->foreign_domain_file = string_init(ASSET_GFW_LIST);
    diverter->domestic_domain_file = string_init(ASSET_CHINA_LIST);

    char *gfwlist = string_join(WORK_DIR, ASSET_GFW_LIST);
    char *china_ip = string_join(WORK_DIR, ASSET_CHINA_IP);
    char *chinalist = string_join(WORK_DIR, ASSET_CHINA_LIST);
    save_string_list(gfwlist, config->diverter.gfwlist);
    save_string_list(china_ip, config->diverter.china_ip);
    save_string_list(chinalist, config->diverter.chinalist);
    free(chinalist);
    free(china_ip);
    free(gfwlist);

    diverter->reject_type = uint32_list_update(diverter->reject_type, config->reject);
    load_diverter_assets();
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

    // TODO: extract assets file
    // TODO: mkdir -p ${WORK_DIR}

    if (!config->adguard.enable) {
        config->diverter.port = config->port; // override diverter port by dns port
    }
    loader.domestic = load_domestic(config);
    loader.foreign = load_foreign(config);
    loader.diverter = load_diverter(config);
    loader.filter = load_filter(config);
    config_free(config);
}
