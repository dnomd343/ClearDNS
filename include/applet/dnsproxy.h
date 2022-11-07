#ifndef DNSPROXY_H_
#define DNSPROXY_H_

#include <stdint.h>
#include "process.h"

typedef struct {
    uint16_t port;
    uint32_t cache;
    uint8_t ipv6; // bool value
    uint8_t debug; // bool value
    uint8_t verify; // bool value
    uint8_t parallel; // bool value
    uint8_t optimistic; // bool value
    char **bootstrap;
    char **fallback;
    char **primary;
} dnsproxy;

void dnsproxy_free(dnsproxy *info);
dnsproxy* dnsproxy_init(uint16_t port);
process* dnsproxy_load(const char *caption, dnsproxy *info, const char *file);

#endif
