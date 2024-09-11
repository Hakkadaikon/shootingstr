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

static void logdump(const enum LogKind kind, PDumpCallback callback, const char* format, ...)
{
    va_list args;
    char    buffer[4096];

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    callback(kind, buffer);
    va_end(args);
}

#endif
