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

bool validate_nostr_event(const PNostrEventObj obj);

#endif
