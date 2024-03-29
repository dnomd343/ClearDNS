#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "loader.h"
#include "logger.h"
#include "parser.h"
#include "sundry.h"
#include "system.h"
#include "default.h"
#include "dnsproxy.h"
#include "constant.h"
#include "structure.h"

struct cleardns loader;

void load_diverter_asset(const char *file) {
    char *src_file = string_join(ASSETS_DIR, file);
    char *dst_file = string_join(WORK_DIR, file);
    file_append(dst_file, src_file);
    free(dst_file);
    free(src_file);
}

void load_diverter_assets() {
    load_diverter_asset(ASSET_CHINA_IP);
    load_diverter_asset(ASSET_GFW_LIST);
    load_diverter_asset(ASSET_CHINA_LIST);
}

dnsproxy* load_domestic(cleardns_config *config) {
    dnsproxy *domestic = dnsproxy_init(config->domestic.port);
    domestic->ipv6 = config->domestic.ipv6;
    domestic->verify = config->domestic.verify;
    domestic->parallel = config->domestic.parallel;
    if (config->cache.enable) {
        domestic->cache = config->cache.size;
        domestic->optimistic = config->cache.optimistic;
    }
    string_list_update(&domestic->bootstrap, config->domestic.bootstrap);
    string_list_update(&domestic->fallback, config->domestic.fallback);
    string_list_update(&domestic->primary, config->domestic.primary);
    return domestic;
}

dnsproxy* load_foreign(cleardns_config *config) {
    dnsproxy *foreign = dnsproxy_init(config->foreign.port);
    foreign->ipv6 = config->foreign.ipv6;
    foreign->verify = config->foreign.verify;
    foreign->parallel = config->foreign.parallel;
    if (config->cache.enable) {
        foreign->cache = config->cache.size;
        foreign->optimistic = config->cache.optimistic;
    }
    string_list_update(&foreign->bootstrap, config->foreign.bootstrap);
    string_list_update(&foreign->fallback, config->foreign.fallback);
    string_list_update(&foreign->primary, config->foreign.primary);
    return foreign;
}

overture* load_diverter(cleardns_config *config) {
    overture *diverter = overture_init();
    diverter->port = config->diverter.port;
    diverter->foreign_port = config->foreign.port;
    diverter->domestic_port = config->domestic.port;

    if (string_list_len(config->ttl)) {
        free(diverter->ttl_file);
        diverter->ttl_file = strdup(ASSET_TTL);
        char *ttl_file = string_join(WORK_DIR, ASSET_TTL);
        save_string_list(ttl_file, config->ttl);
        free(ttl_file);
    }
    if (string_list_len(config->hosts)) {
        free(diverter->host_file);
        diverter->host_file = strdup(ASSET_HOSTS);
        char *hosts_file = string_join(WORK_DIR, ASSET_HOSTS);
        save_string_list(hosts_file, config->hosts);
        free(hosts_file);
    }

    free(diverter->domestic_ip_file);
    free(diverter->foreign_domain_file);
    free(diverter->domestic_domain_file);
    diverter->domestic_ip_file = strdup(ASSET_CHINA_IP);
    diverter->foreign_domain_file = strdup(ASSET_GFW_LIST);
    diverter->domestic_domain_file = strdup(ASSET_CHINA_LIST);

    char *gfwlist = string_join(WORK_DIR, ASSET_GFW_LIST);
    char *china_ip = string_join(WORK_DIR, ASSET_CHINA_IP);
    char *chinalist = string_join(WORK_DIR, ASSET_CHINA_LIST);
    save_string_list(gfwlist, config->diverter.gfwlist);
    save_string_list(china_ip, config->diverter.china_ip);
    save_string_list(chinalist, config->diverter.chinalist);
    free(chinalist);
    free(china_ip);
    free(gfwlist);

    custom_gfwlist = config->diverter.gfwlist;
    custom_china_ip = config->diverter.china_ip;
    custom_chinalist = config->diverter.chinalist;
    config->diverter.gfwlist = string_list_init();
    config->diverter.china_ip = string_list_init();
    config->diverter.chinalist = string_list_init();

    uint32_list_update(&diverter->reject_type, config->reject);
    if (!config->assets.disable) {
        assets_extract(); // extract built-in resource
        load_diverter_assets();
    }
    return diverter;
}

adguard* load_filter(cleardns_config *config) {
    if (!config->adguard.enable) {
        return NULL; // disable adguard
    }
    adguard *filter = adguard_init();
    filter->dns_port = config->port;
    filter->web_port = config->adguard.port;
    filter->username = strdup(config->adguard.username);
    filter->password = strdup(config->adguard.password);
    char *diverter_port = uint32_to_string(config->diverter.port);
    filter->upstream = string_join("127.0.0.1:", diverter_port);
    free(diverter_port);
    return filter;
}

crontab* load_crond(cleardns_config *config) {
    if (config->assets.disable) {
        return NULL; // disable crond
    }
    crontab *crond = crontab_init();
    crond->cron = strdup(config->assets.cron);
    return crond;
}

asset** load_assets(cleardns_config *config) {
    asset **resources = assets_init();
    for (asset **res = config->assets.resources; *res != NULL; ++res) {
        assets_append(&resources, *res); // pointer movement
    }
    *(config->assets.resources) = NULL; // disable old assets list
    return resources;
}

void load_config(const char *config_file) { // parser and load cleardns configure
    cleardns_config *config = config_init();
    load_default_config(config_file); // load default configure
    config_parser(config, config_file); // configure parser
    config_dump(config);

    log_info("Loading configure options");
    if (!config->adguard.enable) {
        config->diverter.port = config->port; // override diverter port by dns port
        log_warn("Diverter port change -> %u", config->diverter.port);
    }

    loader.domestic = load_domestic(config);
    log_debug("Domestic options parser success");

    loader.foreign = load_foreign(config);
    log_debug("Foreign options parser success");

    loader.diverter = load_diverter(config);
    log_debug("Diverter options parser success");

    loader.filter = load_filter(config);
    log_debug("Filter options parser success");

    loader.crond = load_crond(config);
    log_debug("Crond options parser success");

    loader.resource = load_assets(config);
    log_debug("Assets options parser success");

    loader.script = string_list_init();
    string_list_update(&loader.script, config->script);
    log_debug("Custom script parser success");

    config_free(config);
}
