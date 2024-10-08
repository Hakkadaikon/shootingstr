/**
 * @file  websockets.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include "websockets.h"

#include <libwebsockets.h>
#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

/**
 * @brief callback function
 *
 * @param [in] wsi    [description]
 * @param [in] reason [description]
 * @param [in] user   [description]
 * @param [in] data   [description]
 * @param [in] length [description]
 *
 * @return [description]
 * @retval value [return value description]
 */
static int libwebsockets_callback(
    struct lws*               wsi,
    enum lws_callback_reasons reason,
    void*                     user,
    void*                     data,
    size_t                    length);

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

/**
 * @brief websocket protocols
 * - char*                 name
 * - lws_callback_function callback
 * - size_t                per_session_data_size
 * - size_t                rx_buffer_size
 * - unsigned int          id
 * - void*                 user
 * - size_t                tx_packet_size
 */
static struct lws_protocols g_protocols[] = {
    {"websocket-protocol",
     libwebsockets_callback,
     0,
     4096,
     0,
     "user1",
     4096},
    {NULL, NULL, 0, 0, 0, NULL, 0} /* terminator */
};

/**
 * @brief [summary]
 */
static int g_signaled = 0;

/**
 * @brief [summary]
 */
static struct lws* g_wsi = NULL;

/**
 * @brief user callback
 */
static PWebSocketReceiveCallback user_callback = NULL;

/*----------------------------------------------------------------------------*/
/* Static functions                                                           */
/*----------------------------------------------------------------------------*/

static int libwebsockets_callback(
    struct lws*               wsi,
    enum lws_callback_reasons reason,
    void*                     user,
    void*                     data,
    size_t                    length)
{
    g_wsi           = wsi;
    char* char_data = (char*)data;

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            websocket_logdump(LogKindUser, "Connection established\n");
            break;

        case LWS_CALLBACK_RECEIVE:
            if (user_callback == NULL) {
                break;
            }

            if (char_data[length - 1] == '\n') {
                char_data[length - 1] = '\0';
            } else {
                char_data[length] = '\0';
            }

            user_callback(user, char_data);
            break;

        case LWS_CALLBACK_CLOSED:
            websocket_logdump(LogKindUser, "Connection established\n");
            break;

        default:
            break;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

bool websocket_init(PWebSocketInfo websocket)
{
    if (websocket == NULL ||
        websocket->callback == NULL) {
        return false;
    }

    websocket->impl         = malloc(sizeof(WebSocketInfoImpl));
    PWebSocketInfoImpl impl = websocket->impl;

    impl->info.port      = websocket->port;
    impl->info.protocols = g_protocols;
    impl->info.gid       = websocket->gid;
    impl->info.uid       = websocket->uid;
    impl->context        = lws_create_context(&impl->info);
    user_callback        = websocket->callback;

    //lws_set_log_level(LLL_DEBUG | LLL_USER | LLL_ERR, NULL);
    lws_set_log_level(LLL_USER | LLL_ERR, NULL);

    if (impl->context == NULL) {
        websocket_logdump(LogKindError, "lws_create_context failed\n");
        return false;
    }

    websocket_logdump(LogKindUser, "Starting server...\n");
    return true;
}

void websocket_setsignal()
{
    g_signaled = 1;
}

bool websocket_loop(PWebSocketInfo websocket)
{
    if (websocket == NULL ||
        websocket->impl == NULL ||
        websocket->impl->context == NULL) {
        return false;
    }

    websocket_logdump(LogKindUser, "Websocket loop...\n");
    while (!g_signaled) {
        lws_service(websocket->impl->context, 1000);
    }

    websocket_logdump(LogKindUser, "signaled!\n");
    return true;
}

bool websocket_deinit(PWebSocketInfo websocket)
{
    if (websocket == NULL ||
        websocket->impl == NULL ||
        websocket->impl->context == NULL) {
        return false;
    }

    websocket_logdump(LogKindUser, "Websocket deinit...\n");
    lws_context_destroy(websocket->impl->context);
    free(websocket->impl);
    websocket->impl = NULL;
    return true;
}

void websocket_logdump_callback(const enum LogKind kind, const char* str)
{
    switch (kind) {
        case LogKindDebug:
            lwsl_debug(str);
            break;
        case LogKindUser:
            lwsl_user(str);
            break;
        case LogKindError:
            lwsl_err(str);
            break;
        default:
            break;
    }
}

bool websocket_send(const char* buf, const size_t len)
{
    if (buf == NULL || len == 0) {
        return false;
    }

    unsigned char write_buf[LWS_PRE + len];
    memcpy(&write_buf[LWS_PRE], buf, len);
    lws_write(g_wsi, &write_buf[LWS_PRE], len, LWS_WRITE_TEXT);
    return true;
}
