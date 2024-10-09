/**
 * @file  nostr_types.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/

#include "nostr_utils.h"

#include "../utils/compatibility.h"

/*----------------------------------------------------------------------------*/
/* Static variables                                                           */
/*----------------------------------------------------------------------------*/
static PNostrEventSendCallback g_event_send_callback;
static PNostrLogDumpCallback   g_log_dump_callback;

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
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

bool send_ok_message(
    const char* id,
    const bool  accepted,
    const char* reason)
{
    char buf[4096];
    snprintf(buf, 4096, "[\"OK\",\"%s\",%s,\"%s\"]", id, (accepted) ? "true" : "false", reason);

    return nostr_event_send(buf, strnlen(buf, 4096));
}

bool send_eose_message(const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id)
{
    char buf[4096];
    snprintf(buf, 4096, "[\"EOSE\",\"%s\"]", sub_id);

    return nostr_event_send(buf, strnlen(buf, 4096));
}
