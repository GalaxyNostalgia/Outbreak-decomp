#include "log.h"
#include <stdarg.h>
#include <stdio.h>

static void vlog(const char *tag, const char *fmt, va_list ap) {
    fprintf(stderr, "[%s] ", tag);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
}

void log_info(const char *fmt, ...) { va_list ap; va_start(ap, fmt); vlog("info", fmt, ap); va_end(ap); }
void log_warn(const char *fmt, ...) { va_list ap; va_start(ap, fmt); vlog("warn", fmt, ap); va_end(ap); }
void log_error(const char *fmt, ...) { va_list ap; va_start(ap, fmt); vlog("error", fmt, ap); va_end(ap); }
