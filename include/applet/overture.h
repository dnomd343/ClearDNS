#ifndef _OVERTURE_H_
#define _OVERTURE_H_

#include <stdint.h>
#include "process.h"

typedef struct {
    uint16_t port;
    uint8_t debug; // bool value
    uint32_t timeout;
    char *ttl_file;
    char *host_file;
    uint16_t foreign_port;
    uint16_t domestic_port;
    uint32_t **reject_type;
    char *foreign_ip_file;
    char *domestic_ip_file;
    char *foreign_domain_file;
    char *domestic_domain_file;
} overture;

overture* overture_init(uint16_t port);
process* overture_load(overture *info, const char *file);

#endif
