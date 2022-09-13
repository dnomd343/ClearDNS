#include "adguard.h"

adguard* adguard_init() {
    adguard *info = (adguard *)malloc(sizeof(adguard));
    info->debug = FALSE;
    info->dns_port = DNS_PORT;
    info->web_port = ADGUARD_PORT;
    info->upstream = string_join("127.0.0.1:", "5353"); // TODO: use DIVERTER_PORT
    info->username = string_init(ADGUARD_USER);
    info->password = string_init(ADGUARD_PASSWD);
    return info;
}
