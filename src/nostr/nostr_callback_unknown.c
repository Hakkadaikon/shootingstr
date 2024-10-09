/**
 * @file  nostr.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include "nostr.h"
#include "nostr_utils.h"

/*----------------------------------------------------------------------------*/
/* Static functions                                                           */
/*----------------------------------------------------------------------------*/

bool nostr_callback_unknown(void* root)
{
    nostr_logdump(LogKindUser, "Unknown event type\n");
    return true;
}
