/**
 * @file main.c
 * @brief entry point
 */

#include "websockets/websockets.h"
#include <yyjson.h>

int user_callback(
    void*          user,
    const char*    data,
    const int      write_buffer_len,
    unsigned char* write_buffer)
{
    websocket_printf("test user_callback\n");
    websocket_printf("user_callback data : [%s]", data);

    snprintf((char*)write_buffer, write_buffer_len, "test reply");
    return 0;
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
