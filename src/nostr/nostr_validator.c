/**
 * @file  nostr_validator.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/

#include "nostr_validator.h"

#include "nostr_json_wrapper.h"
#include "nostr_utils.h"

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

bool validate_nostr_event(const void* obj)
{
    PNostrEventObj event_obj = (PNostrEventObj)obj;

    bool is_success = true;

    if (!IS_TYPE_NOSTR_EVENT_ID(event_obj->id)) {
        nostr_logevterr("event id is not string.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_PUBLICKEY(event_obj->pubkey)) {
        nostr_logevterr("event pubkey is not string.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_TIME(event_obj->created_at)) {
        nostr_logevterr("event created_at is not number.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_KIND(event_obj->kind)) {
        nostr_logevterr("event kind is not number.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_CONTENT(event_obj->content)) {
        nostr_logevterr("event content is not string.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_TAGS(event_obj->tags)) {
        nostr_logevterr("event tags is not array.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_SIGNATURE(event_obj->sig)) {
        nostr_logevterr("event sig is not string.");
        is_success = false;
    }

    return is_success;
}
