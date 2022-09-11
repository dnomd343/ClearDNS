#ifndef _STRLIST_H_
#define _STRLIST_H_

char* string_init(char *str);

char** string_list_init();
int string_list_len(char **string_list);
char* string_list_dump(char **string_list);
char** string_list_append(char **string_list, char *string);

#endif
