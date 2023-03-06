#ifndef ASSETS_H_
#define ASSETS_H_

typedef struct {
    char *file; // string
    char **sources; // string list
} asset;

asset* asset_init(const char *name);
//void asset_add_src(asset *res, const char *src);

asset** assets_init();
void assets_append(asset ***asset_list, asset *res);

void assets_free(asset **asset_list);

//void asset_free(asset *res);
uint32_t assets_size(asset **asset_list);

void assets_dump(asset **asset_list);

//typedef struct {
//    char **update_file;
//    char **update_url;
//} assets;

//assets* assets_init();
//void assets_free(assets *info);
//void assets_load(assets *info);

void assets_extract();

void assets_log_init(uint8_t verbose);

uint8_t rust_assets_update(const char *file, char *const *sources, const char *assets_dir);

#endif
