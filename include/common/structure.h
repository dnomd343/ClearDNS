#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include <stdint.h>

char** string_list_init();
void string_list_free(char **string_list);
char* string_list_dump(char **string_list);
uint32_t string_list_len(char **string_list);
void string_list_update(char ***base_list, char **update_list);
void string_list_append(char ***string_list, const char *string);

uint32_t** uint32_list_init();
char* uint32_list_dump(uint32_t **int_list);
void uint32_list_free(uint32_t **uint32_list);
uint32_t uint32_list_len(uint32_t **int_list);
void uint32_list_append(uint32_t ***uint32_list, uint32_t number);
void uint32_list_update(uint32_t ***base_list, uint32_t **update_list);

#endif
