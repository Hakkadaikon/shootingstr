/**
 * @file main.c
 * @brief entry point
 */

#include "websockets/websockets.h"
#include <yyjson.h>

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

     websocket.port = 8080;
     websocket.gid  = -1;
     websocket.uid  = -1;

    if (websocket_init(&websocket) != ErrorCodeNone) {
        return 1;
    }

    websocket_loop(&websocket);
    websocket_deinit(&websocket);
    return 0;
}
