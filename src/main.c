/**
 * @file main.c
 * @brief entry point
 */

#include <signal.h>
#include <unistd.h>

#include "nostr/nostr.h"
#include "nostr/nostr_utils.h"
#include "websockets/websockets.h"

/*----------------------------------------------------------------------------*/
/* Static variables                                                           */
/*----------------------------------------------------------------------------*/
static WebSocketInfo websocket;

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

static void signal_handle(int signal)
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

static int websocket_callback(void* user, const char* data)
{
    return nostr_callback(data);
}

static bool nostr_event_send_callback(const char* buf, const size_t len)
{
    return websocket_send(buf, len);
}

static void nostr_logdump_callback(const enum LogKind kind, const char* str)
{
    websocket_logdump(kind, str);
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
    websocket.callback = websocket_callback;

    if (
        (signal(SIGTERM, signal_handle) == SIG_ERR) ||
        (signal(SIGHUP, signal_handle) == SIG_ERR)) {
        return 1;
    }

    if (!websocket_init(&websocket)) {
        return 1;
    }

    if (!nostr_utils_init(nostr_event_send_callback, nostr_logdump_callback)) {
        websocket_deinit(&websocket);
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
