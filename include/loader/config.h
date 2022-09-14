#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

typedef struct {
    uint16_t port;
    uint8_t verify; // bool value
    uint8_t parallel; // bool value
    char **bootstrap;
    char **fallback;
    char **primary;
} upstream_config;

typedef struct {
    uint16_t port;
    char **gfwlist;
    char **china_ip;
    char **chinalist;
} diverter_config;

typedef struct {
    uint32_t size;
    uint8_t enable; // bool value
    uint8_t optimistic; // bool value
} cache_config;

typedef struct {
    uint16_t port;
    uint8_t enable;
    char *username;
    char *password;
} adguard_config;

typedef struct {
    uint16_t port;
    cache_config cache;
    upstream_config domestic;
    upstream_config foreign;
    diverter_config diverter;
    adguard_config adguard;
    uint32_t **reject;
    char **hosts;
    char **ttl;
} cleardns_config;

cleardns_config* config_init();
void config_dump(cleardns_config *config);
void config_free(cleardns_config *config);

#endif
