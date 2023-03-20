#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "logger.h"
#include "system.h"

#define DEFAULT_CONFIG "\
port: 53\n\
\n\
cache:\n\
  enable: true\n\
  size: 4194304\n\
  optimistic: true\n\
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
\n\
assets:\n\
  cron: \"0 4 * * *\"\n\
  update:\n\
    gfwlist.txt: https://cdn.dnomd343.top/cleardns/gfwlist.txt\n\
    china-ip.txt: https://cdn.dnomd343.top/cleardns/china-ip.txt\n\
    chinalist.txt: https://cdn.dnomd343.top/cleardns/chinalist.txt\n\
"

void load_default_config(const char *config_file) {
    if (is_file_exist(config_file)) {
        log_debug("Configure file exist -> skip loading default");
        return;
    }
    log_info("Loading default configure file");
    char *config_content = NULL;
    if (is_json_suffix(config_file)) { // convert to json format
        config_content = to_json_format(DEFAULT_CONFIG);
    } else {
        config_content = strdup(DEFAULT_CONFIG);
    }
    save_file(config_file, config_content);
    free(config_content);
}
