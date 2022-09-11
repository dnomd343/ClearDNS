#ifndef _DNSPROXY_H_
#define _DNSPROXY_H_

typedef struct {
    int port;
    int verify; // bool value
    int parallel; // bool value
    int optimistic; // bool value
    char **bootstrap;
    char **fallback;
    char **primary;
    char *cache;
} dnsproxy;

dnsproxy* dnsproxy_init(int port);
void dnsproxy_dump(char *caption, dnsproxy *info);

#endif
