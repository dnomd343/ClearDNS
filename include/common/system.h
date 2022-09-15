#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>

char* read_file(const char *file);
void remove_file(const char *file);
int run_command(const char *command);
void create_folder(const char *folder);
uint8_t is_file_exist(const char *file);
void save_file(const char *file, const char *content);
void save_string_list(const char *file, char **string_list);

#endif
