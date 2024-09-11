/**
 * @file  nostr_storage.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <rocksdb/c.h>
#include "nostr_types.h"
#include "nostr_storage.h"
#include "../websockets/websockets.h"

/*----------------------------------------------------------------------------*/
/* Types                                                                      */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Structs                                                                    */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Static variables                                                           */
/*----------------------------------------------------------------------------*/
static rocksdb_t*              nostrdb       = NULL;
static rocksdb_options_t*      options       = NULL;
static rocksdb_writeoptions_t* write_options = NULL;

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

bool nostr_storage_init()
{
    options = rocksdb_options_create();
    rocksdb_options_set_create_if_missing(options, 1);
    rocksdb_options_set_info_log_level(options, 4);

    write_options = rocksdb_writeoptions_create();
    rocksdb_writeoptions_set_sync(write_options, 0);

    char* err        = NULL;
    bool  is_success = true;
    // FIXME: dir path
    nostrdb = rocksdb_open(options, "/app/data/nostrdb", &err);
    if (err != NULL) {
        websocket_printf("db open error! reason : [%s]\n", err);
        nostr_storage_deinit();
        is_success = false;
        rocksdb_free(err);
    }

    return is_success;
}

bool save_nostr_event(PNostrEvent event, char* raw_data)
{
    char* err = NULL;

    rocksdb_put(
        nostrdb,
        write_options,
        event->id,
        strlen(event->id),
        raw_data,
        strlen(raw_data),
        &err);

    bool is_success = (err == NULL);
    if (!is_success) {
        websocket_printf("save error! reason : [%s]\n", err);
        rocksdb_free(err);
    }

    return is_success;
}

bool nostr_storage_deinit()
{
    if (options != NULL) {
        rocksdb_options_destroy(options);
    }

    if (write_options != NULL) {
        rocksdb_writeoptions_destroy(write_options);
    }

    if (nostrdb != NULL) {
        rocksdb_close(nostrdb);
    }

    return true;
}
