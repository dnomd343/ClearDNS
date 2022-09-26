#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "constant.h"
#include "structure.h"

char **update_file;
char **update_url;

void assets_update();
void extract_asset(const char *file);

void assets_init() { // init assets and load update process
    log_info("Start loading assets");
    create_folder(ASSETS_DIR);
    extract_asset(ASSET_GFW_LIST);
    extract_asset(ASSET_CHINA_IP);
    extract_asset(ASSET_CHINA_LIST);
    log_info("Assets loaded complete");

    update_url = string_list_init();
    update_file = string_list_init();
    signal(SIGALRM, assets_update); // receive SIGALRM signal
}

// TODO: inject update file and url

void assets_update() { // update all assets
    log_info("Start assets update");
    for (char **file = update_file; *file != NULL; ++file) {
        char **url = file - update_file + update_url;
        log_info("Update asset `%s` -> %s", *file, *url);
        download_file(*file, *url); // download asset from url
    }
    log_info("Restart overture");
    run_command("pgrep overture | xargs kill"); // restart overture
    log_info("Assets update complete");
}

void extract_asset(const char *file) {
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
