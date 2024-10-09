/**
 * @file  nostr.h
 * @brief [description]
 */

#ifndef _SHOOTING_STR_NOSTR_H_
#define _SHOOTING_STR_NOSTR_H_

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

bool nostr_callback_event(void* root);
bool nostr_callback_req(void* root);
bool nostr_callback_close(void* root);
bool nostr_callback_unknown(void* root);
bool nostr_init();
bool nostr_deinit();
bool nostr_callback(const char* data);

#endif
