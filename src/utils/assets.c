#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "constant.h"

void load_asset(const char *file);

// TODO: assets update -> crontab task

void load_assets() {
    log_info("Start loading assets");
    create_folder(ASSETS_DIR);
    load_asset(ASSET_GFW_LIST);
    load_asset(ASSET_CHINA_IP);
    load_asset(ASSET_CHINA_LIST);
    log_info("Assets loaded complete");
}

void load_asset(const char *file) {
    log_debug("Start extract `%s`", file);
    char *output_file = string_join(ASSETS_DIR, file);
    if (is_file_exist(output_file)) {
        log_debug("Assets `%s` exist -> skip extract", file);
        free(output_file);
        return;
    }
    free(output_file);

    char *extract_cmd = (char *)malloc(strlen(ASSETS_PKG) + strlen(file) + strlen(ASSETS_DIR) + 15);
    sprintf(extract_cmd, "tar xf %s ./%s -C %s", ASSETS_PKG, file, ASSETS_DIR);
    if (run_command(extract_cmd)) {
        log_warn("Extract asset `%s` failed", file);
    } else {
        log_info("Extract asset `%s` success", file);
    }
    free(extract_cmd);
}
