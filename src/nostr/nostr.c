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
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <yyjson.h>

/*----------------------------------------------------------------------------*/
/* Types                                                                      */
/*----------------------------------------------------------------------------*/

enum NOSTR_ERROR_CODE {
    NostrErrorCodeNone = 0,
    NostrErrorCodeEventDataIsNotJson,
    NostrErrorCodeIdIsNotStr,
    NostrErrorCodePubkeyIsNotStr,
    NostrErrorCodeCreatedAtIsNotNum,
    NostrErrorCodeKindIsNotNum,
    NostrErrorCodeContentIsNotStr,
    NostrErrorCodeTagIsNotArray,
    NostrErrorCodeSigIsNotStr
};

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Structs                                                                    */
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
    const int   max_write_buffer_len,
    const char* sub_id,
    char*       write_buffer)
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
    yyjson_val* id_obj,
    yyjson_val* pubkey_obj,
    yyjson_val* created_at_obj,
    yyjson_val* kind_obj,
    yyjson_val* tags_obj,
    yyjson_val* content_obj,
    yyjson_val* sig_obj)
{
    int event_err = 0;
    if (!yyjson_is_str(id_obj)) {
        set_nostr_error(NostrErrorCodeIdIsNotStr, &event_err);
    }
    if (!yyjson_is_str(pubkey_obj)) {
        set_nostr_error(NostrErrorCodePubkeyIsNotStr, &event_err);
    }
    if (!yyjson_is_num(created_at_obj)) {
        set_nostr_error(NostrErrorCodeCreatedAtIsNotNum, &event_err);
    }
    if (!yyjson_is_num(kind_obj)) {
        set_nostr_error(NostrErrorCodeKindIsNotNum, &event_err);
    }
    if (!yyjson_is_str(content_obj)) {
        set_nostr_error(NostrErrorCodeContentIsNotStr, &event_err);
    }
    if (!yyjson_is_arr(tags_obj)) {
        set_nostr_error(NostrErrorCodeTagIsNotArray, &event_err);
    }
    if (!yyjson_is_str(sig_obj)) {
        set_nostr_error(NostrErrorCodeSigIsNotStr, &event_err);
    }

    return event_err;
}

static void nostr_callback_event(
    yyjson_val* root,
    const int   max_write_buffer_len,
    char*       write_buffer)
{
    yyjson_val* event_data = yyjson_arr_get(root, 1);
    if (!yyjson_is_obj(event_data)) {
        websocket_printf("EVENT data is not json\n");
        set_nostr_error(NostrErrorCodeEventDataIsNotJson, NULL);
        return;
    }

    yyjson_val* id_obj         = yyjson_obj_get(event_data, "id");
    yyjson_val* pubkey_obj     = yyjson_obj_get(event_data, "pubkey");
    yyjson_val* created_at_obj = yyjson_obj_get(event_data, "created_at");
    yyjson_val* kind_obj       = yyjson_obj_get(event_data, "kind");
    yyjson_val* tags_obj       = yyjson_obj_get(event_data, "tags");
    yyjson_val* content_obj    = yyjson_obj_get(event_data, "content");
    yyjson_val* sig_obj        = yyjson_obj_get(event_data, "sig");

    int event_err = validate_nostr_event(
        id_obj,
        pubkey_obj,
        created_at_obj,
        kind_obj,
        tags_obj,
        content_obj,
        sig_obj);

    const char*   id         = yyjson_get_str(id_obj);
    const char*   pubkey     = yyjson_get_str(pubkey_obj);
    const int64_t created_at = yyjson_get_sint(created_at_obj);
    const int     kind       = yyjson_get_int(kind_obj);
    const char*   content    = yyjson_get_str(content_obj);
    const char*   sig        = yyjson_get_str(sig_obj);
    //TODO: tags

    const char* accepted = (event_err == 0) ? "true" : "false";
    const char* reason   = (event_err == 0) ? "" : "error: event data is broken";

    send_ok_message(max_write_buffer_len, id, accepted, reason, write_buffer);
    return;
}

static void nostr_callback_req(
    yyjson_val* root,
    const int   max_write_buffer_len,
    char*       write_buffer)
{
    // get sub_id
    yyjson_val* sub_id_obj = yyjson_arr_get(root, 1);
    if (!yyjson_is_str(sub_id_obj)) {
        websocket_printf("sub_id is not str.\n");
        return;
    }
    const char* sub_id = yyjson_get_str(sub_id_obj);

    send_eose_message(max_write_buffer_len, sub_id, write_buffer);
    return;
}

static void nostr_callback_close(
    yyjson_val* root,
    const int   max_write_buffer_len,
    char*       write_buffer)
{
    // get sub_id
    yyjson_val* sub_id_obj = yyjson_arr_get(root, 1);
    if (!yyjson_is_str(sub_id_obj)) {
        websocket_printf("sub_id is not str.\n");
        return;
    }
    const char* sub_id = yyjson_get_str(sub_id_obj);

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
    size_t      len = strlen(data);
    yyjson_doc* doc = yyjson_read(data, len, 0);
    if (doc == NULL) {
        websocket_printf("yyjson_read failed.\n");
        goto FINALIZE;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        websocket_printf("yyjson_get_root failed.\n");
        goto FINALIZE;
    }

    yyjson_val* event_type_obj = yyjson_arr_get(root, 0);
    if (!yyjson_is_str(event_type_obj)) {
        websocket_printf("event type is not str.\n");
        goto FINALIZE;
    }

    const char* event_type = yyjson_get_str(event_type_obj);

    if (strstr(event_type, "EVENT")) {
        nostr_callback_event(root, max_write_buffer_len, write_buffer);
    } else if (strstr(event_type, "REQ")) {
        nostr_callback_req(root, max_write_buffer_len, write_buffer);
    } else if (strstr(event_type, "CLOSE")) {
        nostr_callback_close(root, max_write_buffer_len, write_buffer);
    }

FINALIZE:
    if (doc != NULL) {
        yyjson_doc_free(doc);
    }

    return 0;
}
