/**
 * @file  nostr_callback_event.c
 * @brief [description]
 */
/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include "nostr.h"
#include "nostr_json_wrapper.h"
#include "nostr_storage.h"
#include "nostr_types.h"
#include "nostr_utils.h"
#include "nostr_validator.h"

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

bool nostr_callback_event(void* root)
{
    PNOSTR_OBJ event_data = GET_OBJ_NOSTR_MESSAGE_EVENT(root);
    if (!IS_TYPE_NOSTR_MESSAGE_EVENT(event_data)) {
        nostr_logevterr("EVENT data is not json");
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
        event.created_at = GET_NOSTR_EVENT_TIME(obj.created_at);
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
