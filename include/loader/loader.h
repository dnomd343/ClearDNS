#ifndef _LOADER_H_
#define _LOADER_H_

#include "dnsproxy.h"
#include "overture.h"

struct {
    dnsproxy *domestic;
    dnsproxy *foreign;
    overture *diverter;
    // adguard
} loader;

void load_config(const char *config_file);

#endif
