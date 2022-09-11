#ifndef _COMMON_H_
#define _COMMON_H_

#define VERSION "1.3.0-dev"

#define TRUE  1
#define FALSE 0

#define DOMESTIC_PORT 4053
#define FOREIGN_PORT  6053

#define DNSPROXY_BIN "dnsproxy"
#define OVERTURE_BIN "overture"
#define ADGUARD_BIN  "AdGuardHome"


//extern char **adguard_command;
//extern char **overture_command;
//extern char **domestic_dnsproxy_command;
//extern char **foreign_dnsproxy_command;

//void load_start_command(char *adguard_workdir, char *overture_config, char *upstream_config, int is_debug);

char* show_bool(int value);
char** command_init(char *bin);

#endif
