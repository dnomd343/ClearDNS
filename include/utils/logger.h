#ifndef _LOGGER_H_
#define _LOGGER_H_

enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};

#define log_debug(...) log_printf(LOG_DEBUG, __VA_ARGS__)
#define log_info(...) log_printf(LOG_INFO,  __VA_ARGS__)
#define log_warn(...) log_printf(LOG_WARN,  __VA_ARGS__)
#define log_error(...) log_printf(LOG_ERROR, __VA_ARGS__)
#define log_fatal(...) log_printf(LOG_FATAL, __VA_ARGS__)

extern int LOG_LEVEL;
void log_perror(char *prefix);
void log_printf(int level, const char *fmt, ...);

#endif
