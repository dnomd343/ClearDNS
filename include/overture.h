#ifndef _OVERTURE_H_
#define _OVERTURE_H_

#include "process.h"

typedef struct {
    int port;
    int debug;
    int timeout;
    char *ttl_file;
    char *host_file;
    int foreign_port;
    int domestic_port;
    int **reject_type;
    char *foreign_ip_file;
    char *domestic_ip_file;
    char *foreign_domain_file;
    char *domestic_domain_file;
} overture;

overture* overture_init(int port);
process* overture_load(overture *info, const char *file);

#endif
