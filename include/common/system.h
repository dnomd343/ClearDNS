#ifndef _SYSTEM_H_
#define _SYSTEM_H_

char* read_file(const char *file);
int run_command(const char *command);
void create_folder(const char *folder);
void save_file(const char *file, const char *content);
void save_string_list(const char *file, char **string_list);

#endif
