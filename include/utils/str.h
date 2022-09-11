#ifndef _STR_H_
#define _STR_H_

char* string_init(char *str);
char* string_join(char *base, const char *add, int is_new);

char** string_list_init();
int string_list_len(char **string_list);
char* string_list_dump(char **string_list);
char** string_list_append(char **string_list, char *string);

#endif
