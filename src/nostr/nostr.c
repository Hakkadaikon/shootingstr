/**
 * @file  nostr.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include "nostr.h"

#include <stdio.h>

#include "nostr_json_wrapper.h"
#include "nostr_storage.h"
#include "nostr_utils.h"

/*----------------------------------------------------------------------------*/
/* Types                                                                      */
/*----------------------------------------------------------------------------*/

typedef bool (*NOSTR_CALLBACK)(void* root);

/*----------------------------------------------------------------------------*/
/* Static functions                                                           */
/*----------------------------------------------------------------------------*/

static NOSTR_CALLBACK get_nostr_callback(const char* type_str)
{
    if (strstr(type_str, "EVENT")) {
        return nostr_callback_event;
    } else if (strstr(type_str, "REQ")) {
        return nostr_callback_req;
    } else if (strstr(type_str, "CLOSE")) {
        return nostr_callback_close;
    }

    return nostr_callback_unknown;
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

    is_success = get_nostr_callback(GET_NOSTR_MESSAGE_TYPE(event_type_obj))(root);

FINALIZE:
    if (doc != NULL) {
        FREE_NOSTR_DOC(doc);
    }

    return is_success;
}
