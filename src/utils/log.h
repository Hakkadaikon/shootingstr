/**
 * @file  log.h
 * @brief For compatibility with some C functions
 */

#ifndef _SHOOTING_STR_LOG_H_
#define _SHOOTING_STR_LOG_H_

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

enum LogKind {
    LogKindDebug,
    LogKindUser,
    LogKindError
};

typedef void (*PDumpCallback)(const enum LogKind kind, const char* str);

static void logdump(
    const enum LogKind kind,
    PDumpCallback      callback,
    const char*        file_name,
    const int          file_line,
    const char*        format, ...)
{
    va_list args;
    char    format2[4096];
    char    buffer[4096];

    snprintf(format2, 4096, "[%s:%d] %s", file_name, file_line, format);
    va_start(args, format2);
    vsnprintf(buffer, sizeof(buffer), format2, args);
    callback(kind, buffer);
    va_end(args);
}

#endif
