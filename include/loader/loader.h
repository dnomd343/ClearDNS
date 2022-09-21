#ifndef LOADER_H_
#define LOADER_H_

#include "config.h"
#include "adguard.h"
#include "dnsproxy.h"
#include "overture.h"

struct cleardns {
    char **script;
    dnsproxy *domestic;
    dnsproxy *foreign;
    overture *diverter;
    adguard *filter;
    assets_config *assets;
};

extern struct cleardns loader;

void load_config(const char *config_file);

#endif
