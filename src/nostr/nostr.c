/**
 * @file  nostr.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include "nostr.h"
#include "../websockets/websockets.h"
#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include "nostr_types.h"
#include "nostr_validator.h"
#include "nostr_storage.h"

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

typedef void (*NOSTR_CALLBACK)(
    PNOSTR_OBJ root,
    const int  max_write_buffer_len,
    char*      write_buffer);

/*----------------------------------------------------------------------------*/
/* Static variables                                                           */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Static functions                                                           */
/*----------------------------------------------------------------------------*/

static int send_ok_message(
    const int   max_write_buffer_len,
    const char* id,
    const char* accepted,
    const char* reason,
    char*       write_buffer)
{
    return snprintf(write_buffer, max_write_buffer_len, "[\"OK\",\"%s\",%s,\"%s\"]", id, accepted, reason);
}

static int send_eose_message(
    const int                           max_write_buffer_len,
    const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id,
    char*                               write_buffer)
{
    return snprintf(write_buffer, max_write_buffer_len, "[\"EOSE\",\"%s\"]", sub_id);
}

static void nostr_callback_event(
    PNOSTR_OBJ root,
    const int  max_write_buffer_len,
    char*      write_buffer)
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

    int event_err = validate_nostr_event(&obj);

    if (!IS_TYPE_NOSTR_EVENT_ID(obj.id)) {
        return;
    }

    NostrEvent event;

    event.id = GET_NOSTR_EVENT_ID(obj.id);
    if (!event_err) {
        event.pubkey     = GET_NOSTR_EVENT_PUBLICKEY(obj.pubkey);
        event.created_at = GET_NOSTR_EVENT_CREATED_AT(obj.created_at);
        event.kind       = GET_NOSTR_EVENT_KIND(obj.kind);
        event.content    = GET_NOSTR_EVENT_CONTENT(obj.content);
        event.sig        = GET_NOSTR_EVENT_SIGNATURE(obj.sig);
        // event.tags    =  ??? TODO

        save_nostr_event(&event);
    }

    const char* accepted = (event_err == 0) ? "true" : "false";
    const char* reason   = (event_err == 0) ? "" : "error: event data is broken";

    send_ok_message(max_write_buffer_len, event.id, accepted, reason, write_buffer);
    return;
}

static void nostr_callback_req(
    PNOSTR_OBJ root,
    const int  max_write_buffer_len,
    char*      write_buffer)
{
    // get sub_id
    PNOSTR_OBJ sub_id_obj = GET_OBJ_NOSTR_MESSAGE_SUBSCRIPTION_ID(root);
    if (!IS_TYPE_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj)) {
        websocket_printf("sub_id is not str.\n");
        return;
    }
    const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id =
        GET_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj);

    send_eose_message(max_write_buffer_len, sub_id, write_buffer);
    return;
}

static void nostr_callback_close(
    PNOSTR_OBJ root,
    const int  max_write_buffer_len,
    char*      write_buffer)
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
    snprintf(write_buffer, max_write_buffer_len, "[\"EOSE\",\"%s\"]", sub_id);
    return;
}

static void nostr_callback_unknown(
    PNOSTR_OBJ root,
    const int  max_write_buffer_len,
    char*      write_buffer)
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

int nostr_callback(
    const char* data,
    const int   max_write_buffer_len,
    char*       write_buffer)
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

    callbacks[event_type](root, max_write_buffer_len, write_buffer);

FINALIZE:
    if (doc != NULL) {
        FREE_NOSTR_DOC(doc);
    }

    return 0;
}
