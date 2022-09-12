#ifndef _STR_H_
#define _STR_H_

char* string_init(const char *str);
char* string_join(const char *base, const char *add);

int** int_list_init();
int int_list_len(int **int_list);
char* int_list_dump(int **int_list);
int** int_list_append(int **int_list, int number);

char** string_list_init();
int string_list_len(char **string_list);
char* string_list_dump(char **string_list);
char** string_list_append(char **string_list, const char *string);

#endif
