/**
 * @file  nostr_storage.h
 * @brief [description]
 */
#ifndef _SHOOTING_STR_NOSTR_STORAGE_H_
#define _SHOOTING_STR_NOSTR_STORAGE_H_

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "nostr_types.h"

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

bool nostr_storage_init();
bool save_nostr_event(PNostrEvent event, char* raw_data);
bool nostr_storage_deinit();

#endif
