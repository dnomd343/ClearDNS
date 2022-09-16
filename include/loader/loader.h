#ifndef LOADER_H_
#define LOADER_H_

#include "adguard.h"
#include "dnsproxy.h"
#include "overture.h"

struct cleardns {
    dnsproxy *domestic;
    dnsproxy *foreign;
    overture *diverter;
    adguard *filter;
    char **script;
};

extern struct cleardns loader;

void load_config(const char *config_file);

#endif
