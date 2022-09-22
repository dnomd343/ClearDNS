#ifndef CRONTAB_H_
#define CRONTAB_H_

#include "config.h"
#include "process.h"

void assets_free(assets_config *info);
process* assets_load(assets_config *info);

#endif
