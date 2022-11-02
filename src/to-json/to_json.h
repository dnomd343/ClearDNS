#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void free_rust_string(const char *string);

const char *to_json_rust(const char *content);
