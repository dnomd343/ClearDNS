#ifndef SUNDRY_H_
#define SUNDRY_H_

#include <stdint.h>

char* show_bool(uint8_t value);
uint8_t check_port(uint16_t port);
char* string_load(const char *fmt, ...);
char* uint32_to_string(uint32_t number);
char* string_join(const char *base, const char *add);
void string_list_debug(char *describe, char **string_list);
void uint32_list_debug(char *describe, uint32_t **uint32_list);

#endif
