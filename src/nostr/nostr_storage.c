/**
 * @file  nostr_storage.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <rocksdb/c.h>
#include "nostr_types.h"
#include "nostr_utils.h"
#include "nostr_storage.h"

/*----------------------------------------------------------------------------*/
/* Types                                                                      */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Structs                                                                    */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Static variables                                                           */
/*----------------------------------------------------------------------------*/
static rocksdb_t*              g_nostrdb       = NULL;
static rocksdb_options_t*      g_options       = NULL;
static rocksdb_writeoptions_t* g_write_options = NULL;

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

bool nostr_storage_init()
{
    g_options = rocksdb_options_create();
    rocksdb_options_set_create_if_missing(g_options, 1);
    rocksdb_options_set_info_log_level(g_options, 4);

    g_write_options = rocksdb_writeoptions_create();
    rocksdb_writeoptions_set_sync(g_write_options, 0);

    char* err        = NULL;
    bool  is_success = true;
    // FIXME: dir path
    g_nostrdb = rocksdb_open(g_options, "/app/data/nostrdb", &err);
    if (err != NULL) {
        nostr_logdump(LogKindError, "db open error! reason : [%s]\n", err);
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
        g_nostrdb,
        g_write_options,
        event->id,
        strlen(event->id),
        raw_data,
        strlen(raw_data),
        &err);

    bool is_success = (err == NULL);
    if (!is_success) {
        nostr_logdump(LogKindError, "save error! reason : [%s]\n", err);
        rocksdb_free(err);
    }

    return is_success;
}

bool nostr_storage_deinit()
{
    if (g_options != NULL) {
        rocksdb_options_destroy(g_options);
    }

    if (g_write_options != NULL) {
        rocksdb_writeoptions_destroy(g_write_options);
    }

    if (g_nostrdb != NULL) {
        rocksdb_close(g_nostrdb);
    }

    return true;
}
