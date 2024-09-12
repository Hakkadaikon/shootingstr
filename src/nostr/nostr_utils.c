/**
 * @file  nostr_types.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/

#include "nostr_utils.h"

static PNostrEventSendCallback g_event_send_callback;
static PNostrLogDumpCallback   g_log_dump_callback;

/*----------------------------------------------------------------------------*/
/* Types                                                                      */
/*----------------------------------------------------------------------------*/

bool nostr_utils_init(
    PNostrEventSendCallback event_send_callback,
    PNostrLogDumpCallback   log_dump_callback)
{
    if (event_send_callback == NULL || log_dump_callback == NULL) {
        return false;
    }

    g_event_send_callback = event_send_callback;
    g_log_dump_callback   = log_dump_callback;
    return true;
}

bool nostr_event_send(const char* buf, const size_t len)
{
    return g_event_send_callback(buf, len);
}

void nostr_logdump_local(const enum LogKind kind, const char* str)
{
    g_log_dump_callback(kind, str);
}

void nostr_logevterr(const char* reason)
{
    nostr_logdump(LogKindError, "Event error! reason : [%s]\n", reason);
}
