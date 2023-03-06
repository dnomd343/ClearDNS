#ifndef ASSETS_H_
#define ASSETS_H_

typedef struct {
    char *file; // string
    char **sources; // string list
} asset;

asset* asset_init(const char *name);

asset** assets_init();
void assets_dump(asset **asset_list);
void assets_free(asset **asset_list);
uint32_t assets_size(asset **asset_list);
void assets_append(asset ***asset_list, asset *res);

void assets_load(asset **info);

void assets_extract();

//void assets_log_init(uint8_t verbose);
//
//uint8_t rust_assets_update(const char *file, char *const *sources, const char *assets_dir);

#endif
