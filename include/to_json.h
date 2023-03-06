#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Free the exported c-style string.
 */
void free_rust_string(const char *ptr);

/**
 * Format the input text into JSON format and return a c-style string, or return
 * `NULL` if an error occurs.
 */
const char *to_json(const char *content);
