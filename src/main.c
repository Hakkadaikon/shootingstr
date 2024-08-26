/**
 * @file main.c
 * @brief entry point
 */

#include "websockets/websockets.h"
#include "nostr/nostr.h"
#include <yyjson.h>

int user_callback(
    void*       user,
    const char* data,
    const int   max_write_buffer_len,
    char*       write_buffer)
{
    return nostr_callback(data, max_write_buffer_len, write_buffer);
}

/**
 * @brief entry point
 *
 * @param [in] argc [description]
 * @param [in] argv [description]
 *
 * @return [description]
 * @retval value [return value description]
 */
int main(int argc, char** argv)
{
    WebSocketInfo websocket;

    websocket.port     = 8080;
    websocket.gid      = -1;
    websocket.uid      = -1;
    websocket.callback = user_callback;

    if (websocket_init(&websocket) != ErrorCodeNone) {
        return 1;
    }

    websocket_loop(&websocket);
    websocket_deinit(&websocket);
    return 0;
}
