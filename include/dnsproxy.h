#ifndef _DNSPROXY_H_
#define _DNSPROXY_H_

typedef struct {
    int port;
    int cache;
    int verify; // bool value
    int parallel; // bool value
    int optimistic; // bool value
    char **bootstrap;
    char **fallback;
    char **primary;
} dnsproxy;

dnsproxy* dnsproxy_init(int port);
void dnsproxy_dump(char *caption, dnsproxy *info);

void dnsproxy_add_primary(dnsproxy *info, char *server);
void dnsproxy_add_fallback(dnsproxy *info, char *server);
void dnsproxy_add_bootstrap(dnsproxy *info, char *server);

void dnsproxy_gen_config(dnsproxy *info);

#endif
