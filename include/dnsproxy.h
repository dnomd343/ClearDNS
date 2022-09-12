#ifndef _DNSPROXY_H_
#define _DNSPROXY_H_

#include "process.h"

typedef struct {
    int port;
    int cache;
    int debug; // bool value
    int verify; // bool value
    int parallel; // bool value
    int optimistic; // bool value
    char **bootstrap;
    char **fallback;
    char **primary;
} dnsproxy;

dnsproxy* dnsproxy_init(int port);
void dnsproxy_add_primary(dnsproxy *info, const char *server);
void dnsproxy_add_fallback(dnsproxy *info, const char *server);
void dnsproxy_add_bootstrap(dnsproxy *info, const char *server);
process* dnsproxy_load(const char *caption, dnsproxy *info, const char *file);

#endif
