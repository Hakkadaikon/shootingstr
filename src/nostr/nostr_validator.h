/**
 * @file  nostr_validator.h
 * @brief [description]
 */
#ifndef _SHOOTING_STR_NOSTR_VALIDATOR_H_
#define _SHOOTING_STR_NOSTR_VALIDATOR_H_

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/

#include "nostr_types.h"
#include "../websockets/websockets.h"

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

void set_nostr_error(const char* reason)
{
    websocket_printf("Event error! reason : [%s]\n", reason);
}

int validate_nostr_event(
    const PNostrEventObj obj)
{
    int event_err = 0;
    if (!IS_TYPE_NOSTR_EVENT_ID(obj->id)) {
        set_nostr_error("event id is not string.");
        event_err = 1;
    }
    if (!IS_TYPE_NOSTR_EVENT_PUBLICKEY(obj->pubkey)) {
        set_nostr_error("event pubkey is not string.");
        event_err = 1;
    }
    if (!IS_TYPE_NOSTR_EVENT_CREATED_AT(obj->created_at)) {
        set_nostr_error("event created_at is not number.");
        event_err = 1;
    }
    if (!IS_TYPE_NOSTR_EVENT_KIND(obj->kind)) {
        set_nostr_error("event kind is not number.");
        event_err = 1;
    }
    if (!IS_TYPE_NOSTR_EVENT_CONTENT(obj->content)) {
        set_nostr_error("event content is not string.");
        event_err = 1;
    }
    if (!IS_TYPE_NOSTR_EVENT_TAGS(obj->tags)) {
        set_nostr_error("event tags is not array.");
        event_err = 1;
    }
    if (!IS_TYPE_NOSTR_EVENT_SIGNATURE(obj->sig)) {
        set_nostr_error("event sig is not string.");
        event_err = 1;
    }

    return event_err;
}

#endif
