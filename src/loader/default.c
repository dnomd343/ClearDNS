#include <stdlib.h>
#include "json.h"
#include "logger.h"
#include "system.h"
#include "sundry.h"

#define DEFAULT_CONFIG "\
port: 53\n\
\n\
diverter:\n\
  port: 5353\n\
\n\
adguard:\n\
  enable: true\n\
  port: 80\n\
  username: admin\n\
  password: cleardns\n\
\n\
cache:\n\
  enable: true\n\
  size: 4194304\n\
  optimistic: true\n\
\n\
domestic:\n\
  port: 4053\n\
  bootstrap: 223.5.5.5\n\
  primary:\n\
    - tls://dns.alidns.com\n\
    - https://doh.pub/dns-query\n\
  fallback:\n\
    - 223.6.6.6\n\
    - 119.29.29.29\n\
\n\
foreign:\n\
  port: 6053\n\
  bootstrap: 8.8.8.8\n\
  primary:\n\
    - tls://dns.google\n\
    - https://dns.cloudflare.com/dns-query\n\
  fallback:\n\
    - 1.1.1.1\n\
    - 8.8.4.4\n\
"

void load_default_config(const char *config_file) {
    if (is_file_exist(config_file)) {
        log_debug("Configure file exist -> skip load default");
        return;
    }
    log_info("Loading default configure file");
    char *config_content = NULL;
    if (is_json_suffix(config_file)) { // convert to json format
        char temp_file[] = "temp.yml";
        save_file(temp_file, DEFAULT_CONFIG);
        config_content = to_json(temp_file);
        remove_file(temp_file);
    }
    if (config_content == NULL) {
        config_content = string_init(DEFAULT_CONFIG);
    }
    save_file(config_file, config_content);
    free(config_content);
}
