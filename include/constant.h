#ifndef CONSTANT_H_
#define CONSTANT_H_

#define TRUE  1
#define FALSE 0

#define VERSION "1.3.0-dev"
#define CONFIG_FILE "cleardns.yml"

#define DNS_PORT      53
#define ADGUARD_PORT  80
#define DIVERTER_PORT 5353
#define DOMESTIC_PORT 4053
#define FOREIGN_PORT  6053

#define DIVERTER_TIMEOUT 6

#define ADGUARD_USER   "admin"
#define ADGUARD_PASSWD "cleardns"

#define DNSPROXY_BIN "dnsproxy"
#define OVERTURE_BIN "overture"
#define ADGUARD_BIN  "AdGuardHome"

#define EXPOSE_DIR  "/cleardns/"
#define WORK_DIR    "/etc/cleardns/"
#define ASSETS_DIR  "/cleardns/assets/"
#define ADGUARD_DIR "/cleardns/adguard/"

#define UPDATE_CRON      "0 4 * * *"
#define ASSETS_PKG       "/assets.tar.xz"

#define ASSET_TTL        "ttl.txt"
#define ASSET_HOSTS      "hosts.txt"
#define ASSET_GFW_LIST   "gfwlist.txt"
#define ASSET_CHINA_IP   "china-ip.txt"
#define ASSET_CHINA_LIST "chinalist.txt"

#define EXIT_NORMAL 0
#define EXIT_FILE_ERROR 1
#define EXIT_FORK_ERROR 2
#define EXIT_EXEC_ERROR 3
#define EXIT_WAIT_ERROR 4

#define HELP_MSG "\
ClearDNS usage \n\
--debug ...\n\
--config ...\n\
--help ...\n\
"

#endif
