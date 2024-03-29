#ifndef LOADER_H_
#define LOADER_H_

#include "config.h"
#include "adguard.h"
#include "dnsproxy.h"
#include "overture.h"
#include "crontab.h"
#include "assets.h"

struct cleardns {
    char **script;
    dnsproxy *domestic;
    dnsproxy *foreign;
    overture *diverter;
    adguard *filter;
    crontab *crond;
    asset **resource;
};

extern struct cleardns loader;

void load_diverter_assets();

void load_config(const char *config_file);

#endif
