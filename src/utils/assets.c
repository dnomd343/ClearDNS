#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "assets.h"
#include "loader.h"
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "constant.h"
#include "structure.h"

asset **update_info;

char **custom_gfwlist;
char **custom_china_ip;
char **custom_chinalist;

void assets_update_entry();
void extract(const char *file);

asset* asset_init(const char *name) { // init asset item
    asset *res = (asset *)malloc(sizeof(asset));
    res->file = strdup(name);
    res->sources = string_list_init(); // with multi sources
    return res;
}

asset** assets_init() { // init assets list
    asset **asset_list = (asset **)malloc(sizeof(asset *));
    *asset_list = NULL; // list end sign
    return asset_list;
}

void assets_free(asset **asset_list) { // free assets list
    for (asset **res = asset_list; *res != NULL; ++res) {
        string_list_free((*res)->sources);
        free((*res)->file);
        free(*res);
    }
    free(asset_list);
}

uint32_t assets_size(asset **asset_list) { // get size of assets list
    uint32_t num = 0;
    while(asset_list[num++] != NULL); // get list size
    return num - 1;
}

void assets_dump(asset **asset_list) { // dump assets list content into debug log
    for (asset **res = asset_list; *res != NULL; ++res) { // iterate over each item
        char *sources = string_list_dump((*res)->sources);
        log_debug("Asset item `%s` -> %s", (*res)->file, sources);
        free(sources);
    }
}

void assets_append(asset ***asset_list, asset *res) { // push asset item for assets list
    uint32_t len = assets_size(*asset_list);
    *asset_list = (asset **)realloc(*asset_list, sizeof(asset *) * (len + 2)); // extend asset list
    (*asset_list)[len] = res;
    (*asset_list)[len + 1] = NULL; // list end sign
}

void assets_load(asset **info) { // load assets list
    update_info = assets_init();
    for (asset **res = info; *res != NULL; ++res) {
        assets_append(&update_info, *res); // pointer movement
    }
    *info = NULL; // disable old assets list
    assets_dump(update_info);
    log_info("Remote assets load success");
    signal(SIGALRM, assets_update_entry); // receive SIGALRM signal
}

void assets_update_entry() { // receive SIGALRM for update all assets
    if (assets_size(update_info) == 0) { // empty assets list
        log_info("Skip update assets");
        return;
    }
    log_info("Start updating assets");
    for (asset **res = update_info; *res != NULL; ++res) {
        char *content = string_list_dump((*res)->sources);
        log_debug("Updating `%s` -> %s", (*res)->file, content);
        if (asset_update((*res)->file, (*res)->sources, ASSETS_DIR)) {
            log_debug("Asset `%s` update success", (*res)->file);
        } else {
            log_warn("Asset `%s` update failed", (*res)->file);
        }
        free(content);
    }

    char *gfwlist = string_join(WORK_DIR, ASSET_GFW_LIST);
    char *china_ip = string_join(WORK_DIR, ASSET_CHINA_IP);
    char *chinalist = string_join(WORK_DIR, ASSET_CHINA_LIST);
    save_string_list(gfwlist, custom_gfwlist);
    save_string_list(china_ip, custom_china_ip);
    save_string_list(chinalist, custom_chinalist);
    free(chinalist);
    free(china_ip);
    free(gfwlist);
    load_diverter_assets(); // load assets data into `WORK_DIR`

    log_info("Restart overture to apply new assets");
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
