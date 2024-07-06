/**
 * @file  websockets.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include "websockets.h"
#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

/**
 * @brief callback function
 *
 * @param [in] wsi    [description]
 * @param [in] reason [description]
 * @param [in] user   [description]
 * @param [in] in     [description]
 * @param [in] len    [description]
 *
 * @return [description]
 * @retval value [return value description]
 */
static int callback_websockets(
    struct lws*               wsi,
    enum lws_callback_reasons reason,
    void*                     user,
    void*                     in,
    size_t                    len);

/*----------------------------------------------------------------------------*/
/* Structs                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * @brief websocket info
 */
struct _WebSocketInfoImpl {
    /**
     * @var [description]
     */
    struct lws_context_creation_info info;

    /**
     * @var [description]
     */
    struct lws_context* context;
};

/*----------------------------------------------------------------------------*/
/* Static variables                                                           */
/*----------------------------------------------------------------------------*/

static struct lws_protocols protocols[] = {
    {
        "websocket-protocol",
        callback_websockets,
        0,
        4096,
    },
    {NULL, NULL, 0, 0} /* terminator */
};

/*----------------------------------------------------------------------------*/
/* Static functions                                                           */
/*----------------------------------------------------------------------------*/

static int callback_websockets(
    struct lws*               wsi,
    enum lws_callback_reasons reason,
    void*                     user,
    void*                     in,
    size_t                    len)
{
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            lwsl_user("Connection established\n");
            break;

        case LWS_CALLBACK_RECEIVE:
            lwsl_user("Received data: %s\n", (char*)in);
            break;

        case LWS_CALLBACK_CLOSED:
            lwsl_user("Connection closed\n");
            break;
        default:
            break;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

enum WEB_SOCKET_ERROR_CODE websocket_init(
    PWebSocketInfo websocket)
{
    if (websocket == NULL) {
        return ErrorCodeInvalidArgument;
    }

    websocket->impl         = malloc(sizeof(WebSocketInfoImpl));
    PWebSocketInfoImpl impl = websocket->impl;

    impl->info.port      = websocket->port;
    impl->info.protocols = protocols;
    impl->info.gid       = websocket->gid;
    impl->info.uid       = websocket->uid;
    impl->context        = lws_create_context(&impl->info);

    if (impl->context == NULL) {
        lwsl_err("lws_create_context failed\n");
        return ErrorCodeInitializeSocket;
    }

    lwsl_user("Starting server...\n");
    return ErrorCodeNone;
}

void websocket_loop(PWebSocketInfo websocket)
{
    if (
        websocket == NULL ||
        websocket->impl == NULL ||
        websocket->impl->context == NULL) {
        return;
    }

    lwsl_user("Websocket loop...\n");
    while (1) {
        lws_service(websocket->impl->context, 1000);
    }
}

void websocket_deinit(PWebSocketInfo websocket)
{
    if (
        websocket == NULL ||
        websocket->impl == NULL ||
        websocket->impl->context == NULL) {
        return;
    }

    lwsl_user("deinit...\n");
    lws_context_destroy(websocket->impl->context);
    free(websocket->impl);
    websocket->impl = NULL;
}
