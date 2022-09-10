#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "logger.h"

int LOG_LEVEL = LOG_INFO; // default log level

static const char *log_string[] = {
    "[DEBUG]",
    "[INFO]",
    "[WARN]",
    "[ERROR]",
    "[FATAL]",
};

static const char *log_color[] = {
    "\x1b[39m", // debug
    "\x1b[32m", // info
    "\x1b[33m", // warn
    "\x1b[31m", // error
    "\x1b[95m", // fatal
};

void log_printf(int level, const char *fmt, ...) {
    if (level < LOG_LEVEL) { // skip low log level
        return;
    }
    time_t t = time(NULL);
    char time_str[20]; // YYYY-mm-dd HH:MM:SS (20 bytes)
    time_str[strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", localtime(&t))] = '\0'; // generate time str

    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "\x1b[36m[Bootstrap]\x1b[0m \x1b[90m%s\x1b[0m", time_str); // show log prefix
    fprintf(stderr, " %s%s\x1b[0m ", log_color[level], log_string[level]); // show log level
    vfprintf(stderr, fmt, ap); // output log content
    fprintf(stderr, "\n"); // add LF after line
    fflush(stderr);
    va_end(ap);

    if (level == LOG_FATAL) {
        exit(1);
    }
}

void log_perror(char *prefix) {
    time_t t;
    time(&t);
    struct tm *lt = localtime(&t);
    fprintf(stderr, "\x1b[36m[Bootstrap]\x1b[0m ");
    fprintf(stderr, "\x1b[90m%04d-%02d-%02d %02d:%02d:%02d\x1b[0m",
            lt->tm_year + 1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
    fprintf(stderr, " %s%s\x1b[0m ", log_color[3], log_string[3]); // error level
    fflush(stderr);
    perror(prefix);
}
