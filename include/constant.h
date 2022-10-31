#ifndef CONSTANT_H_
#define CONSTANT_H_

#define TRUE  1
#define FALSE 0

#define DNS_PORT      53
#define ADGUARD_PORT  80
#define DIVERTER_PORT 5353
#define DOMESTIC_PORT 4053
#define FOREIGN_PORT  6053

#define RESTART_DELAY    1
#define DIVERTER_TIMEOUT 6

#define VERSION     "1.3.3"
#define CONFIG_FILE "cleardns.yml"

#define DNSPROXY_BIN "dnsproxy"
#define OVERTURE_BIN "overture"
#define ADGUARD_BIN  "AdGuardHome"

#define UPDATE_CRON "0 4 * * *"
#define ASSETS_PKG  "/assets.tar.xz"

#define ADGUARD_USER   "admin"
#define ADGUARD_PASSWD "cleardns"

#define EXPOSE_DIR  "/cleardns/"
#define WORK_DIR    "/etc/cleardns/"
#define ASSETS_DIR  "/cleardns/assets/"
#define ADGUARD_DIR "/cleardns/adguard/"

#define ASSET_TTL        "ttl.txt"
#define ASSET_HOSTS      "hosts.txt"
#define ASSET_GFW_LIST   "gfwlist.txt"
#define ASSET_CHINA_IP   "china-ip.txt"
#define ASSET_CHINA_LIST "chinalist.txt"

#define EXIT_NORMAL 0
#define EXIT_FORK_ERROR 1
#define EXIT_EXEC_ERROR 2
#define EXIT_WAIT_ERROR 3

#define HELP_MSG "\
ClearDNS usage:\n\
\n\
  Command: ./cleardns [OPTIONS]\n\
\n\
  Options:\n\
    --debug   Enable debug mode.\n\
    --config  Specify config file. (default: cleardns.yml)\n\
    --version Show version information.\n\
    --help    Print this message.\n\
"

#endif
