/**
 * @file  nostr_types.h
 * @brief [description]
 */
#ifndef _SHOOTING_STR_UTILS_H_
#define _SHOOTING_STR_UTILS_H_

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "nostr_types.h"
#include "../utils/log.h"

typedef bool (*PNostrEventSendCallback)(const char* send_data, const size_t len);
typedef void (*PNostrLogDumpCallback)(const enum LogKind kind, const char* str);

/*----------------------------------------------------------------------------*/
/* Types                                                                      */
/*----------------------------------------------------------------------------*/

bool nostr_utils_init(
    PNostrEventSendCallback event_send_callback,
    PNostrLogDumpCallback   log_dump_callback);

bool nostr_event_send(const char* buf, const size_t len);
void nostr_logdump_local(const enum LogKind kind, const char* str);

#define nostr_logdump(kind, ...) \
    logdump(kind, 4096, nostr_logdump_local, __FILE__, __LINE__, __VA_ARGS__)

void nostr_logevterr(const char* reason);

#endif
