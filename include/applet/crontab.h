#ifndef CRONTAB_H_
#define CRONTAB_H_

#include <stdint.h>
#include "process.h"

typedef struct {
    uint8_t debug; // bool value
    char *cron; // cron expression
} crontab;

void crontab_free(crontab *info);
crontab* crontab_init(char *cron);
process* crontab_load(crontab *info);

#endif
