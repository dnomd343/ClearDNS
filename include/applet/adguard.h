#ifndef ADGUARD_H_
#define ADGUARD_H_

#include <stdint.h>
#include "process.h"

typedef struct {
    uint8_t debug; // bool value
    uint16_t dns_port;
    uint16_t web_port;
    char *upstream;
    char *username;
    char *password;
} adguard;

adguard* adguard_init();
void adguard_free(adguard *info);
process* adguard_load(adguard *info, const char *dir);

#endif
