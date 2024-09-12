/**
 * @file  log.h
 * @brief For compatibility with some C functions
 */

#ifndef _SHOOTING_STR_LOG_H_
#define _SHOOTING_STR_LOG_H_

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

enum LogKind {
    LogKindDebug,
    LogKindUser,
    LogKindError
};

typedef void (*PDumpCallback)(const enum LogKind kind, const char* str);

static void logdump(
    const enum LogKind kind,
    const int          max_buffer_size,
    PDumpCallback      callback,
    const char*        file_name,
    const int          file_line,
    const char*        format,
    ...)
{
    va_list args;
    char    format2[max_buffer_size];
    char    buffer[max_buffer_size];

    snprintf(format2, max_buffer_size, "[%s:%d] %s", file_name, file_line, format);
    va_start(args, format2);
    vsnprintf(buffer, sizeof(buffer), format2, args);
    callback(kind, buffer);
    va_end(args);
}

#endif
