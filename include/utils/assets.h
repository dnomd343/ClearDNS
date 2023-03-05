#ifndef ASSETS_H_
#define ASSETS_H_

typedef struct {
    char **update_file;
    char **update_url;
} assets;

assets* assets_init();
void assets_free(assets *info);
void assets_load(assets *info);

void assets_extract();

// RUST DEMO START

void rust_test(const char *const *ptr);

// RUST DEMO END

#endif
