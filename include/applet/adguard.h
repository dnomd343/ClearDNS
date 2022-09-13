#ifndef _ADGUARD_H_
#define _ADGUARD_H_

#include <stdint.h>

typedef struct {
    uint8_t debug; // bool value
    uint16_t dns_port;
    uint16_t web_port;
    char *upstream;
    char *username;
    char *password;
} adguard;

#endif
