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

int nostr_storage_init()
{
    options = rocksdb_options_create();
    rocksdb_options_set_create_if_missing(options, 1);

    write_options = rocksdb_writeoptions_create();

    char* err = NULL;

    nostrdb = rocksdb_open(options, "nostrdb", &err);
    if (err != NULL) {
        rocksdb_free(err);
        nostr_storage_deinit();
        return 1;
    }

    return 0;
}

int save_nostr_event(PNostrEvent event, char* raw_data)
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

    int ret = (err != NULL);
    if (ret) {
        websocket_printf("save error! reason : [%s]\n", err);
    }
    rocksdb_free(err);

    return ret;
}

int nostr_storage_deinit()
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

    return 0;
}