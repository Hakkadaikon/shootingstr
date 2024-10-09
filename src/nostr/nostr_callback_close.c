/**
 * @file  nostr.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include "nostr.h"
#include "nostr_json_wrapper.h"
#include "nostr_types.h"
#include "nostr_utils.h"

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

bool nostr_callback_close(void* root)
{
    // get sub_id
    PNOSTR_OBJ sub_id_obj = GET_OBJ_NOSTR_MESSAGE_SUBSCRIPTION_ID(root);
    if (!IS_TYPE_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj)) {
        nostr_logdump(LogKindError, "sub_id is not str.\n");
        return false;
    }
    const NOSTR_MESSAGE_SUBSCRIPTION_ID sub_id =
        GET_NOSTR_MESSAGE_SUBSCRIPTION_ID(sub_id_obj);

    // Send EOSE message
    return send_eose_message(sub_id);
}
