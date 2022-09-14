#ifndef _LOADER_H_
#define _LOADER_H_

#include "adguard.h"
#include "dnsproxy.h"
#include "overture.h"

extern struct {
    dnsproxy *domestic;
    dnsproxy *foreign;
    overture *diverter;
    adguard *filter;
} loader;

void load_config(const char *config_file);

#endif
