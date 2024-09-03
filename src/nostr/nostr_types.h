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

typedef yyjson_val NOSTR_OBJ, *PNOSTR_OBJ;
typedef yyjson_doc NOSTR_DOC, *PNOSTR_DOC;

typedef char* NOSTR_MESSAGE_TYPE;
typedef char* NOSTR_MESSAGE_SUBSCRIPTION_ID;

typedef char*   NOSTR_EVENT_ID;
typedef char*   NOSTR_EVENT_PUBLICKEY;
typedef int64_t NOSTR_EVENT_CREATED_AT;
typedef int     NOSTR_EVENT_KIND;
typedef char*   NOSTR_EVENT_CONTENT;
typedef char*   NOSTR_EVENT_SIGNATURE;
typedef char*   NOSTR_EVENT_TAGS;  //TODO

#define GET_NOSTR_MESSAGE_DOC(DATA, LEN) yyjson_read(DATA, LEN, 0)
#define FREE_NOSTR_DOC yyjson_doc_free

#define GET_OBJ_NOSTR_MESSAGE_ROOT yyjson_doc_get_root
#define IS_TYPE_NOSTR_MESSAGE_ROOT yyjson_is_arr

#define GET_OBJ_NOSTR_MESSAGE_TYPE(ROOT) yyjson_arr_get(ROOT, 0)
#define IS_TYPE_NOSTR_MESSAGE_TYPE yyjson_is_str
#define GET_NOSTR_MESSAGE_TYPE yyjson_get_str

#define GET_OBJ_NOSTR_MESSAGE_EVENT(ROOT) yyjson_arr_get(ROOT, 1)
#define IS_TYPE_NOSTR_MESSAGE_EVENT yyjson_is_obj
#define GET_NOSTR_MESSAGE_EVENT yyjson_get_obj

#define GET_OBJ_NOSTR_MESSAGE_SUBSCRIPTION_ID(ROOT) yyjson_arr_get(ROOT, 1)
#define IS_TYPE_NOSTR_MESSAGE_SUBSCRIPTION_ID yyjson_is_str
#define GET_NOSTR_MESSAGE_SUBSCRIPTION_ID yyjson_get_str

#define GET_OBJ_NOSTR_EVENT_ID(EVENT) yyjson_obj_get(EVENT, "id")
#define IS_TYPE_NOSTR_EVENT_ID yyjson_is_str
#define GET_NOSTR_EVENT_ID yyjson_get_str

#define GET_OBJ_NOSTR_EVENT_PUBLICKEY(EVENT) yyjson_obj_get(EVENT, "pubkey")
#define IS_TYPE_NOSTR_EVENT_PUBLICKEY yyjson_is_str
#define GET_NOSTR_EVENT_PUBLICKEY yyjson_get_str

#define GET_OBJ_NOSTR_EVENT_CREATED_AT(EVENT) yyjson_obj_get(EVENT, "created_at")
#define IS_TYPE_NOSTR_EVENT_CREATED_AT yyjson_is_num
#define GET_NOSTR_EVENT_CREATED_AT yyjson_get_sint

#define GET_OBJ_NOSTR_EVENT_KIND(EVENT) yyjson_obj_get(EVENT, "kind")
#define IS_TYPE_NOSTR_EVENT_KIND yyjson_is_num
#define GET_NOSTR_EVENT_KIND yyjson_get_int

#define GET_OBJ_NOSTR_EVENT_CONTENT(EVENT) yyjson_obj_get(EVENT, "content")
#define IS_TYPE_NOSTR_EVENT_CONTENT yyjson_is_str
#define GET_NOSTR_EVENT_CONTENT yyjson_get_str

#define GET_OBJ_NOSTR_EVENT_SIGNATURE(EVENT) yyjson_obj_get(EVENT, "sig")
#define IS_TYPE_NOSTR_EVENT_SIGNATURE yyjson_is_str
#define GET_NOSTR_EVENT_SIGNATURE yyjson_get_str

#define GET_OBJ_NOSTR_EVENT_TAGS(EVENT) yyjson_obj_get(EVENT, "tags")
#define IS_TYPE_NOSTR_EVENT_TAGS yyjson_is_arr
//define GET_NOSTR_EVENT_TAGS TODO

/*----------------------------------------------------------------------------*/
/* Structs                                                                    */
/*----------------------------------------------------------------------------*/

typedef struct _NostrEventObj {
    PNOSTR_OBJ id;
    PNOSTR_OBJ pubkey;
    PNOSTR_OBJ created_at;
    PNOSTR_OBJ kind;
    PNOSTR_OBJ tags;
    PNOSTR_OBJ content;
    PNOSTR_OBJ sig;
} NostrEventObj, *PNostrEventObj;

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
