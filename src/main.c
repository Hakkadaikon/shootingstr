/**
 * @file main.c
 * @brief entry point
 */

#include "websockets/websockets.h"
#include "nostr/nostr.h"
#include <signal.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/*----------------------------------------------------------------------------*/
/* Static variables                                                           */
/*----------------------------------------------------------------------------*/
static WebSocketInfo websocket;

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

void signal_handle(int signal)
{
    switch (signal) {
        case SIGHUP:
        case SIGTERM:
            websocket_setsignal();
            break;

        default:
            break;
    }
}

int user_callback(
    void*       user,
    const char* data)
{
    return nostr_callback(data);
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
    websocket.port     = 8080;
    websocket.gid      = -1;
    websocket.uid      = -1;
    websocket.callback = user_callback;

    if (
        (signal(SIGTERM, signal_handle) == SIG_ERR) ||
        (signal(SIGHUP, signal_handle) == SIG_ERR)) {
        return 1;
    }

    if (!websocket_init(&websocket)) {
        return 1;
    }

    if (!nostr_init()) {
        websocket_deinit(&websocket);
        return 1;
    }

    websocket_loop(&websocket);

    nostr_deinit();
    websocket_deinit(&websocket);
    return 0;
}
