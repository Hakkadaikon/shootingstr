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

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

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

static const char* error_to_string(const enum NOSTR_ERROR_CODE code)
{
    switch (code) {
        case NostrErrorCodeNone:
            return "";
        case NostrErrorCodeEventDataIsNotJson:
            return "Event Error : EVENT data is not json";
        case NostrErrorCodeIdIsNotStr:
            return "EVENT error : id is not str";
        case NostrErrorCodePubkeyIsNotStr:
            return "EVENT error : pubkey is not str";
        case NostrErrorCodeCreatedAtIsNotNum:
            return "EVENT error : created_at is not num";
        case NostrErrorCodeKindIsNotNum:
            return "EVENT error : kind is not num";
        case NostrErrorCodeContentIsNotStr:
            return "EVENT error : content is not str";
        case NostrErrorCodeTagIsNotArray:
            return "EVENT error : tags is not array";
        case NostrErrorCodeSigIsNotStr:
            return "EVENT error : sig is not str";
        default:
            return "Unknown error.";
    }
}

static void set_nostr_error(const enum NOSTR_ERROR_CODE code, int* error)
{
    if (code == NostrErrorCodeNone) {
        return;
    }

    websocket_printf("%s\n", error_to_string(code));
    if (error != NULL) {
        *error = 1;
    }
}

static int validate_nostr_event(
    const PNostrEventObj obj)
{
    int event_err = 0;
    if (!IS_TYPE_NOSTR_EVENT_ID(obj->id)) {
        set_nostr_error(NostrErrorCodeIdIsNotStr, &event_err);
    }
    if (!IS_TYPE_NOSTR_EVENT_PUBLICKEY(obj->pubkey)) {
        set_nostr_error(NostrErrorCodePubkeyIsNotStr, &event_err);
    }
    if (!IS_TYPE_NOSTR_EVENT_CREATED_AT(obj->created_at)) {
        set_nostr_error(NostrErrorCodeCreatedAtIsNotNum, &event_err);
    }
    if (!IS_TYPE_NOSTR_EVENT_KIND(obj->kind)) {
        set_nostr_error(NostrErrorCodeKindIsNotNum, &event_err);
    }
    if (!IS_TYPE_NOSTR_EVENT_CONTENT(obj->content)) {
        set_nostr_error(NostrErrorCodeContentIsNotStr, &event_err);
    }
    if (!IS_TYPE_NOSTR_EVENT_TAGS(obj->tags)) {
        set_nostr_error(NostrErrorCodeTagIsNotArray, &event_err);
    }
    if (!IS_TYPE_NOSTR_EVENT_SIGNATURE(obj->sig)) {
        set_nostr_error(NostrErrorCodeSigIsNotStr, &event_err);
    }

    return event_err;
}

static void nostr_callback_event(
    PNOSTR_OBJ root,
    const int  max_write_buffer_len,
    char*      write_buffer)
{
    PNOSTR_OBJ event_data = GET_OBJ_NOSTR_MESSAGE_EVENT(root);
    if (!IS_TYPE_NOSTR_MESSAGE_EVENT(event_data)) {
        websocket_printf("EVENT data is not json\n");
        set_nostr_error(NostrErrorCodeEventDataIsNotJson, NULL);
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

    NostrEvent event;
    event.id         = GET_NOSTR_EVENT_ID(obj.id);
    event.pubkey     = GET_NOSTR_EVENT_PUBLICKEY(obj.pubkey);
    event.created_at = GET_NOSTR_EVENT_CREATED_AT(obj.created_at);
    event.kind       = GET_NOSTR_EVENT_KIND(obj.kind);
    event.content    = GET_NOSTR_EVENT_CONTENT(obj.content);
    event.sig        = GET_NOSTR_EVENT_SIGNATURE(obj.sig);
    // event.tags    =  ??? TODO

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

    const NOSTR_MESSAGE_TYPE event_type = GET_NOSTR_MESSAGE_TYPE(event_type_obj);

    if (strstr(event_type, "EVENT")) {
        nostr_callback_event(root, max_write_buffer_len, write_buffer);
    } else if (strstr(event_type, "REQ")) {
        nostr_callback_req(root, max_write_buffer_len, write_buffer);
    } else if (strstr(event_type, "CLOSE")) {
        nostr_callback_close(root, max_write_buffer_len, write_buffer);
    }

FINALIZE:
    if (doc != NULL) {
        FREE_NOSTR_DOC(doc);
    }

    return 0;
}
