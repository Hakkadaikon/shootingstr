/**
 * @file  compatibility.h
 * @brief For compatibility with some C functions
 */

#ifndef _SHOOTING_STR_COMPATIBILITY_H_
#define _SHOOTING_STR_COMPATIBILITY_H_

#include <stddef.h>

static inline size_t strnlen(const unsigned char* str, size_t maxlen)
{
    size_t ii;

    for (ii = 0; ii < maxlen; ii++) {
        if (str[ii] == 0) {
            break;
        }
    }

    return ii;
}

#endif
