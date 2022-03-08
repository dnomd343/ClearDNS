#ifndef _COMMON_H_
#define _COMMON_H_

extern char **init_command;
extern char **crond_command;
extern char **adguard_command;
extern char **overture_command;
extern char **domestic_dnsproxy_command;
extern char **foreign_dnsproxy_command;

void load_start_command(char *adguard_workdir, char *overture_config, char *upstream_config, int is_debug);

#endif
