#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
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

void fprint_prefix() { // print log prefix and time info
    time_t _time;
    time(&_time);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *t = localtime(&_time);
    fprintf(stderr, "\x1b[36m[%s]\x1b[0m", LOG_PREFIX);
    fprintf(stderr, " \x1b[90m%04d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    fprintf(stderr, " %02d:%02d:%02d.%03ld\x1b[0m", t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec / 1000);
}

void log_printf(int level, const char *fmt, ...) {
    if (level < LOG_LEVEL) { // skip low log level
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    fprint_prefix();
    fprintf(stderr, " %s%s\x1b[0m ", log_color[level], log_string[level]); // show log level
    vfprintf(stderr, fmt, ap); // output log content
    fprintf(stderr, "\n"); // add LF after line
    fflush(stderr);
    va_end(ap);
    if (level == LOG_FATAL) {
        exit(1);
    }
}

//void log_perror(char *prefix) {
//    fprint_prefix();
//    fprintf(stderr, " %s%s\x1b[0m ", log_color[LOG_ERROR], log_string[LOG_ERROR]);
//    fflush(stderr);
//    perror(prefix);
//}

// TODO: update perror logger

void log_perror(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprint_prefix();
    fprintf(stderr, " %s%s\x1b[0m ", log_color[LOG_ERROR], log_string[LOG_ERROR]);
    vfprintf(stderr, fmt, ap); // output log content
    fflush(stderr);
    perror("");
}
