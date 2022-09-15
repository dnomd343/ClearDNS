#ifndef CONSTANT_H_
#define CONSTANT_H_

#define VERSION "1.3.0-dev"

#define TRUE  1
#define FALSE 0

#define DNS_PORT      53
#define ADGUARD_PORT  80
#define DIVERTER_PORT 5353
#define DOMESTIC_PORT 4053
#define FOREIGN_PORT  6053

#define DIVERTER_TIMEOUT 6

#define ADGUARD_USER   "admin"
#define ADGUARD_PASSWD "adguard"

#define DNSPROXY_BIN "dnsproxy"
#define OVERTURE_BIN "overture"
#define ADGUARD_BIN  "AdGuardHome"

#define WORK_DIR    "/etc/cleardns/"
#define EXPOSE_DIR  "/cleardns/"
#define ASSETS_DIR  "/cleardns/assets/"
#define ADGUARD_DIR "/cleardns/adguard/"

#define ASSET_TTL        "ttl.txt"
#define ASSET_HOSTS      "hosts.txt"
#define ASSET_GFW_LIST   "gfwlist.txt"
#define ASSET_CHINA_IP   "china-ip.txt"
#define ASSET_CHINA_LIST "chinalist.txt"

#endif
