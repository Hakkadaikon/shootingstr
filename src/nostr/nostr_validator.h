/**
 * @file  nostr_validator.h
 * @brief [description]
 */
#ifndef _SHOOTING_STR_NOSTR_VALIDATOR_H_
#define _SHOOTING_STR_NOSTR_VALIDATOR_H_

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/

#include <stdbool.h>
#include "nostr_types.h"

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

static void set_nostr_error(const char* reason)
{
    nostr_logdump(LogKindError, "Event error! reason : [%s]\n", reason);
}

static bool validate_nostr_event(const PNostrEventObj obj)
{
    bool is_success = true;

    if (!IS_TYPE_NOSTR_EVENT_ID(obj->id)) {
        set_nostr_error("event id is not string.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_PUBLICKEY(obj->pubkey)) {
        set_nostr_error("event pubkey is not string.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_CREATED_AT(obj->created_at)) {
        set_nostr_error("event created_at is not number.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_KIND(obj->kind)) {
        set_nostr_error("event kind is not number.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_CONTENT(obj->content)) {
        set_nostr_error("event content is not string.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_TAGS(obj->tags)) {
        set_nostr_error("event tags is not array.");
        is_success = false;
    }
    if (!IS_TYPE_NOSTR_EVENT_SIGNATURE(obj->sig)) {
        set_nostr_error("event sig is not string.");
        is_success = false;
    }

    return is_success;
}

#endif
