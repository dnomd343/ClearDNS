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

void assets_log_init(uint8_t verbose);

uint8_t rust_assets_update(const char *file, char *const *sources, const char *assets_dir);

#endif
