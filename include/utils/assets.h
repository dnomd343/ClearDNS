#ifndef ASSETS_H_
#define ASSETS_H_

#include <stdint.h>

extern char **custom_gfwlist;
extern char **custom_china_ip;
extern char **custom_chinalist;

typedef struct {
    char *file; // string
    char **sources; // string list
} asset;

void assets_extract();
void assets_load(asset **info);

asset** assets_init();
asset* asset_init(const char *name);
void assets_dump(asset **asset_list);
void assets_free(asset **asset_list);
uint32_t assets_size(asset **asset_list);
void assets_append(asset ***asset_list, asset *res);

/// Rust assets interface
void assets_log_init(uint8_t verbose);
uint8_t asset_update(const char *file, char *const *sources, const char *assets_dir);

#endif
