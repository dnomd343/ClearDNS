#ifndef _OVERTURE_H_
#define _OVERTURE_H_

#include "process.h"

typedef struct {
    int port;
    int timeout;
    int foreign_port;
    int domestic_port;
    char *foreign_ip_file;
    char *domestic_ip_file;
    char *foreign_domain_file;
    char *domestic_domain_file;
} overture;

overture* overture_init(int port);
process* overture_load(overture *info, const char *file);

#endif
