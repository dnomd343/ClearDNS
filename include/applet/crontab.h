#ifndef CRONTAB_H_
#define CRONTAB_H_

#include <stdint.h>
#include "process.h"

typedef struct {
    uint8_t debug; // bool value
    char *cron; // cron expression
} crontab;

crontab* crontab_init();
void crontab_free(crontab *info);
process* crontab_load(crontab *info);

#endif
