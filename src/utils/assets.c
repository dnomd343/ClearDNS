#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "constant.h"
#include "structure.h"
#include "assets.h"

assets update;

void assets_update();
void assets_dump(assets *info);
void extract(const char *file);

void assets_free(assets *info) { // free assets mapping
    string_list_free(info->update_file);
    string_list_free(info->update_url);
    free(info);
}

assets* assets_init() { // init assets mapping
    assets *info = (assets *)malloc(sizeof(assets));
    info->update_file = string_list_init();
    info->update_url = string_list_init();
    return info;
}

void assets_dump(assets *info) { // show assets mapping in debug log
    for (char **file = info->update_file; *file != NULL; ++file) {
        char **url = file - info->update_file + info->update_url;
        log_info("Asset `%s` -> %s", *file, *url);
    }
}

void assets_load(assets *info) { // load assets mapping
    update.update_file = string_list_init();
    update.update_url = string_list_init();
    string_list_update(&update.update_file, info->update_file);
    string_list_update(&update.update_url, info->update_url);
    signal(SIGALRM, assets_update); // receive SIGALRM signal
    assets_dump(&update);
}

void assets_update() { // update all assets
    if (!string_list_len(update.update_file)) { // empty assets mapping
        log_info("Skip update assets");
        return;
    }
    log_info("Start assets update");
    for (char **file = update.update_file; *file != NULL; ++file) {
        char **url = file - update.update_file + update.update_url;
        char *asset_file = string_join(ASSETS_DIR, *file);
        log_info("Update asset `%s` -> %s", asset_file, *url);
        download_file(asset_file, *url); // download asset from url
        free(asset_file);
    }
    log_info("Restart overture");
    run_command("pgrep overture | xargs kill"); // restart overture
    log_info("Assets update complete");
}

void assets_extract() { // init assets and load update process
    log_info("Start loading assets");
    create_folder(ASSETS_DIR);
    extract(ASSET_GFW_LIST);
    extract(ASSET_CHINA_IP);
    extract(ASSET_CHINA_LIST);
    log_info("Assets loading complete");
}

void extract(const char *file) { // extract one asset file from `.tar.xz` file
    log_debug("Start extract `%s`", file);
    char *output_file = string_join(ASSETS_DIR, file);
    if (is_file_exist(output_file)) {
        log_debug("Assets `%s` exist -> skip extract", file);
        free(output_file);
        return;
    }
    free(output_file);

    char *extract_cmd = string_load("tar xf %s %s -C %s", ASSETS_PKG, file, ASSETS_DIR);
    if (run_command(extract_cmd)) {
        log_warn("Extract asset `%s` failed", file);
    } else {
        log_info("Extract asset `%s` success", file);
    }
    free(extract_cmd);
}
