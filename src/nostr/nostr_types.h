/**
 * @file  nostr_types.h
 * @brief [description]
 */
#ifndef _SHOOTING_STR_NOSTR_TYPES_H_
#define _SHOOTING_STR_NOSTR_TYPES_H_

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------*/
/* Types                                                                      */
/*----------------------------------------------------------------------------*/

enum NostrMessageType {
    NostrMessageTypeEvent = 0,
    NostrMessageTypeReq,
    NostrMessageTypeClose,
    NostrMessageTypeUnknown,
};

typedef char* NOSTR_MESSAGE_TYPE;
typedef char* NOSTR_MESSAGE_SUBSCRIPTION_ID;

typedef char*   NOSTR_EVENT_ID;
typedef char*   NOSTR_EVENT_PUBLICKEY;
typedef int64_t NOSTR_EVENT_CREATED_AT;
typedef int     NOSTR_EVENT_KIND;
typedef char*   NOSTR_EVENT_CONTENT;
typedef char*   NOSTR_EVENT_SIGNATURE;
typedef char*   NOSTR_EVENT_TAGS;  //TODO

/*----------------------------------------------------------------------------*/
/* Structs                                                                    */
/*----------------------------------------------------------------------------*/

typedef struct _NostrEvent {
    NOSTR_EVENT_ID         id;
    NOSTR_EVENT_PUBLICKEY  pubkey;
    NOSTR_EVENT_CREATED_AT created_at;
    NOSTR_EVENT_KIND       kind;
    NOSTR_EVENT_CONTENT    content;
    NOSTR_EVENT_SIGNATURE  sig;
    NOSTR_EVENT_TAGS       tags;
} NostrEvent, *PNostrEvent;

#endif
