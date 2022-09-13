#ifndef _DNSPROXY_H_
#define _DNSPROXY_H_

#include <stdint.h>
#include "process.h"

typedef struct {
    uint16_t port;
    uint32_t cache;
    uint8_t debug; // bool value
    uint8_t verify; // bool value
    uint8_t parallel; // bool value
    uint8_t optimistic; // bool value
    char **bootstrap;
    char **fallback;
    char **primary;
} dnsproxy;

dnsproxy* dnsproxy_init(uint16_t port);
void dnsproxy_add_primary(dnsproxy *info, const char *server);
void dnsproxy_add_fallback(dnsproxy *info, const char *server);
void dnsproxy_add_bootstrap(dnsproxy *info, const char *server);
process* dnsproxy_load(const char *caption, dnsproxy *info, const char *file);

#endif
