#include "loader.h"
#include "logger.h"
#include "config.h"
#include "parser.h"

void load_dnsproxy() {

}

void load_config(const char *config_file) {
    cleardns_config *config = config_init();
    config_parser(config, config_file);

    config_dump(config);

    // TODO: load into process

    config_free(config);
}
