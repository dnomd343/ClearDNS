#include <stdio.h>
#include <string.h>
#include "to_json.h"

int main() {
    printf("start demo\n");

    char yaml_string[] = "test: ok\narray:\n  - 123\n  - 234\n  - 345\n";
    printf("----------------\n");
    printf("%s", yaml_string);
    printf("----------------\n");

    const char *raw_json_string = to_json_rust(yaml_string);
    char *json_string = strdup(raw_json_string);

    printf("----------------\n");
    printf("%s\n", json_string);
    printf("----------------\n");

    free_rust_string(raw_json_string);
    printf("rust string free complete\n");

    return 0;
}
