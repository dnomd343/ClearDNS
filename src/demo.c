#include <stdio.h>
#include "to_json.h"

int main() {
    printf("start demo\n");

    char raw_string[] = "hello";

    const char *json_content = to_json_rust(raw_string);

    printf("return content -> `%s`\n", json_content);

    free_rust_string(json_content);

    printf("rust string free success\n");

    return 0;
}
