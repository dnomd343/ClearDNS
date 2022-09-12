#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>

typedef u_int8_t uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int64_t uint64_t;

#define VERSION "1.3.0-dev"

#define TRUE  1
#define FALSE 0

#define DOMESTIC_PORT 4053
#define FOREIGN_PORT  6053
#define DIVERTER_PORT 5353

#define DNSPROXY_BIN "dnsproxy"
#define OVERTURE_BIN "overture"
#define ADGUARD_BIN  "AdGuardHome"

#define WORK_DIR "/etc/cleardns/"

//extern char **adguard_command;
//extern char **overture_command;
//extern char **domestic_dnsproxy_command;
//extern char **foreign_dnsproxy_command;

//void load_start_command(char *adguard_workdir, char *overture_config, char *upstream_config, int is_debug);

char* show_bool(int value);
char* read_file(char *file);
void save_file(char *file, char *content);

#endif
