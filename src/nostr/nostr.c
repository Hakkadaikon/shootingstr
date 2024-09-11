/**
 * @file  nostr.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include "nostr.h"
#include "../websockets/websockets.h"
#include "../utils/compatibility.h"
#include <string.h>
#include <stdio.h>
#include <yyjson.h>
#include "nostr_types.h"
#include "nostr_validator.h"
#include "nostr_storage.h"

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

typedef void (*NOSTR_CALLBACK)(PNOSTR_OBJ root);

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

    return websocket_write(buf, strnlen(buf, 4096));
}

static bool send_eose_message(const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id)
{
    char buf[4096];
    snprintf(buf, 4096, "[\"EOSE\",\"%s\"]", sub_id);

    return websocket_write(buf, strnlen(buf, 4096));
}

static void nostr_callback_event(PNOSTR_OBJ root)
{
    PNOSTR_OBJ event_data = GET_OBJ_NOSTR_MESSAGE_EVENT(root);
    if (!IS_TYPE_NOSTR_MESSAGE_EVENT(event_data)) {
        set_nostr_error("EVENT data is not json");
        return;
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
        return;
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
        //websocket_printf("pubkey : %s data : %s\n", event.pubkey, raw_data);

        if (!save_nostr_event(&event, raw_data)) {
            accepted = false;
            reason   = "failed save event data";
        }
    } else {
        reason = "event data is broken";
    }

    send_ok_message(event.id, accepted, reason);
    return;
}

static void nostr_callback_req(PNOSTR_OBJ root)
{
    // get sub_id
    PNOSTR_OBJ sub_id_obj = GET_OBJ_NOSTR_MESSAGE_SUBSCRIPTION_ID(root);
    if (!IS_TYPE_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj)) {
        websocket_printf("sub_id is not str.\n");
        return;
    }
    const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id =
        GET_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj);

    send_eose_message(sub_id);
    return;
}

static void nostr_callback_close(PNOSTR_OBJ root)
{
    // get sub_id
    PNOSTR_OBJ sub_id_obj = GET_OBJ_NOSTR_MESSAGE_SUBSCRIPTION_ID(root);
    if (!IS_TYPE_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj)) {
        websocket_printf("sub_id is not str.\n");
        return;
    }
    const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id =
        GET_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj);

    // Send EOSE message
    send_eose_message(sub_id);
    return;
}

static void nostr_callback_unknown(PNOSTR_OBJ root)
{
    websocket_printf("Unknown event type\n");
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
    websocket_printf("nostr deinit...\n");
    return nostr_storage_deinit();
}

bool nostr_callback(const char* data)
{
    size_t     len = strlen(data);
    PNOSTR_DOC doc = GET_NOSTR_MESSAGE_DOC(data, len);
    if (doc == NULL) {
        websocket_printf("yyjson_read failed.\n");
        goto FINALIZE;
    }

    PNOSTR_OBJ root = GET_OBJ_NOSTR_MESSAGE_ROOT(doc);
    if (!IS_TYPE_NOSTR_MESSAGE_ROOT(root)) {
        websocket_printf("yyjson_get_root failed.\n");
        goto FINALIZE;
    }

    PNOSTR_OBJ event_type_obj = GET_OBJ_NOSTR_MESSAGE_TYPE(root);
    if (!IS_TYPE_NOSTR_MESSAGE_TYPE(event_type_obj)) {
        websocket_printf("event type is not str.\n");
        goto FINALIZE;
    }

    const enum NostrMessageType event_type =
        string_to_message_type(GET_NOSTR_MESSAGE_TYPE(event_type_obj));

    static NOSTR_CALLBACK callbacks[] = {
        nostr_callback_event,
        nostr_callback_req,
        nostr_callback_close,
        nostr_callback_unknown};

    callbacks[event_type](root);

FINALIZE:
    if (doc != NULL) {
        FREE_NOSTR_DOC(doc);
    }

    return true;
}
