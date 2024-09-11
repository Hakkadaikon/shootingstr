/**
 * @file  nostr.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include "nostr.h"
#include "../utils/compatibility.h"
#include "nostr_types.h"
#include "nostr_validator.h"
#include "nostr_storage.h"

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

typedef bool (*NOSTR_CALLBACK)(PNOSTR_OBJ root);

/*----------------------------------------------------------------------------*/
/* Static variables                                                           */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Static functions                                                           */
/*----------------------------------------------------------------------------*/

static bool send_ok_message(
    const char* id,
    const bool  accepted,
    const char* reason)
{
    char buf[4096];
    snprintf(buf, 4096, "[\"OK\",\"%s\",%s,\"%s\"]", id, (accepted) ? "true" : "false", reason);

    return nostr_write(buf, strnlen(buf, 4096));
}

static bool send_eose_message(const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id)
{
    char buf[4096];
    snprintf(buf, 4096, "[\"EOSE\",\"%s\"]", sub_id);

    return nostr_write(buf, strnlen(buf, 4096));
}

static bool nostr_callback_event(PNOSTR_OBJ root)
{
    PNOSTR_OBJ event_data = GET_OBJ_NOSTR_MESSAGE_EVENT(root);
    if (!IS_TYPE_NOSTR_MESSAGE_EVENT(event_data)) {
        set_nostr_error("EVENT data is not json");
        return false;
    }

    NostrEventObj obj;
    obj.id         = GET_OBJ_NOSTR_EVENT_ID(event_data);
    obj.pubkey     = GET_OBJ_NOSTR_EVENT_PUBLICKEY(event_data);
    obj.created_at = GET_OBJ_NOSTR_EVENT_CREATED_AT(event_data);
    obj.kind       = GET_OBJ_NOSTR_EVENT_KIND(event_data);
    obj.tags       = GET_OBJ_NOSTR_EVENT_TAGS(event_data);
    obj.content    = GET_OBJ_NOSTR_EVENT_CONTENT(event_data);
    obj.sig        = GET_OBJ_NOSTR_EVENT_SIGNATURE(event_data);

    const char* reason   = "";
    bool        accepted = validate_nostr_event(&obj);

    if (!IS_TYPE_NOSTR_EVENT_ID(obj.id)) {
        return false;
    }

    NostrEvent event;

    event.id = GET_NOSTR_EVENT_ID(obj.id);
    if (accepted) {
        event.pubkey     = GET_NOSTR_EVENT_PUBLICKEY(obj.pubkey);
        event.created_at = GET_NOSTR_EVENT_CREATED_AT(obj.created_at);
        event.kind       = GET_NOSTR_EVENT_KIND(obj.kind);
        event.content    = GET_NOSTR_EVENT_CONTENT(obj.content);
        event.sig        = GET_NOSTR_EVENT_SIGNATURE(obj.sig);
        // event.tags    =  ??? TODO

        char* raw_data = GET_JSON_RAW_DATA(event_data);

        if (!save_nostr_event(&event, raw_data)) {
            accepted = false;
            reason   = "failed save event data";
        }
    } else {
        reason = "event data is broken";
    }

    return send_ok_message(event.id, accepted, reason);
}

static bool nostr_callback_req(PNOSTR_OBJ root)
{
    // get sub_id
    PNOSTR_OBJ sub_id_obj = GET_OBJ_NOSTR_MESSAGE_SUBSCRIPTION_ID(root);
    if (!IS_TYPE_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj)) {
        nostr_logdump(LogKindError, "sub_id is not str.\n");
        return false;
    }
    const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id =
        GET_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj);

    return send_eose_message(sub_id);
}

static bool nostr_callback_close(PNOSTR_OBJ root)
{
    // get sub_id
    PNOSTR_OBJ sub_id_obj = GET_OBJ_NOSTR_MESSAGE_SUBSCRIPTION_ID(root);
    if (!IS_TYPE_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj)) {
        nostr_logdump(LogKindError, "sub_id is not str.\n");
        return false;
    }
    const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id =
        GET_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj);

    // Send EOSE message
    return send_eose_message(sub_id);
}

static bool nostr_callback_unknown(PNOSTR_OBJ root)
{
    nostr_logdump(LogKindUser, "Unknown event type\n");
    return true;
}

static enum NostrMessageType string_to_message_type(const char* type_str)
{
    if (strstr(type_str, "EVENT")) {
        return NostrMessageTypeEvent;
    } else if (strstr(type_str, "REQ")) {
        return NostrMessageTypeReq;
    } else if (strstr(type_str, "CLOSE")) {
        return NostrMessageTypeClose;
    }

    return NostrMessageTypeUnknown;
}

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

bool nostr_init()
{
    return nostr_storage_init();
}

bool nostr_deinit()
{
    nostr_logdump(LogKindUser, "nostr deinit...\n");
    return nostr_storage_deinit();
}

bool nostr_callback(const char* data)
{
    size_t     len        = strlen(data);
    PNOSTR_DOC doc        = GET_NOSTR_MESSAGE_DOC(data, len);
    bool       is_success = true;

    if (doc == NULL) {
        nostr_logdump(LogKindError, "yyjson_read failed.\n");
        is_success = false;
        goto FINALIZE;
    }

    PNOSTR_OBJ root = GET_OBJ_NOSTR_MESSAGE_ROOT(doc);
    if (!IS_TYPE_NOSTR_MESSAGE_ROOT(root)) {
        nostr_logdump(LogKindError, "yyjson_get_root failed.\n");
        is_success = false;
        goto FINALIZE;
    }

    PNOSTR_OBJ event_type_obj = GET_OBJ_NOSTR_MESSAGE_TYPE(root);
    if (!IS_TYPE_NOSTR_MESSAGE_TYPE(event_type_obj)) {
        nostr_logdump(LogKindError, "event type is not str.\n");
        is_success = false;
        goto FINALIZE;
    }

    const enum NostrMessageType event_type =
        string_to_message_type(GET_NOSTR_MESSAGE_TYPE(event_type_obj));

    static NOSTR_CALLBACK callbacks[] = {
        nostr_callback_event,
        nostr_callback_req,
        nostr_callback_close,
        nostr_callback_unknown};

    is_success = callbacks[event_type](root);

FINALIZE:
    if (doc != NULL) {
        FREE_NOSTR_DOC(doc);
    }

    return is_success;
}
