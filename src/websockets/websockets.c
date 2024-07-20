/**
 * @file  websockets.c
 * @brief [description]
 */

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/
#include "websockets.h"
#include "../utils/compatibility.h"
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
 * @param [in] data   [description]
 * @param [in] length [description]
 *
 * @return [description]
 * @retval value [return value description]
 */
static int callback_websockets(
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
static struct lws_protocols protocols[] = {
    {"websocket-protocol",
     callback_websockets,
     0,
     4096,
     0,
     "user1",
     4096},
    {NULL, NULL, 0, 0, 0, NULL, 0} /* terminator */
};

/**
 * @brief user callback
 */
static WebSocketCallback user_callback = NULL;

/*----------------------------------------------------------------------------*/
/* Static functions                                                           */
/*----------------------------------------------------------------------------*/

static int callback_websockets(
    struct lws*               wsi,
    enum lws_callback_reasons reason,
    void*                     user,
    void*                     data,
    size_t                    length)
{
    const int     MAX_WRITE_BUFFER_LENGTH      = 4096;
    const int     MAX_WRITE_BUFFER_DATA_LENGTH = MAX_WRITE_BUFFER_LENGTH - LWS_PRE;
    unsigned char write_buffer[MAX_WRITE_BUFFER_LENGTH];
    char*         char_data = (char*)data;

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            lwsl_user("Connection established\n");
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

            memset(write_buffer, 0x00, sizeof(write_buffer));
            unsigned char* p_write_data = &write_buffer[LWS_PRE];
            user_callback(user, char_data, MAX_WRITE_BUFFER_DATA_LENGTH, p_write_data);

            if (*p_write_data == 0) {
                break;
            }

            size_t write_len = strnlen(p_write_data, MAX_WRITE_BUFFER_DATA_LENGTH);
            lws_write(wsi, p_write_data, write_len, LWS_WRITE_TEXT);
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
    if (websocket == NULL ||
        websocket->callback == NULL) {
        return ErrorCodeInvalidArgument;
    }

    websocket->impl         = malloc(sizeof(WebSocketInfoImpl));
    PWebSocketInfoImpl impl = websocket->impl;

    impl->info.port      = websocket->port;
    impl->info.protocols = protocols;
    impl->info.gid       = websocket->gid;
    impl->info.uid       = websocket->uid;
    impl->context        = lws_create_context(&impl->info);
    user_callback        = websocket->callback;

    lws_set_log_level(LLL_USER, NULL);

    if (impl->context == NULL) {
        lwsl_err("lws_create_context failed\n");
        return ErrorCodeInitializeSocket;
    }

    lwsl_user("Starting server...\n");
    return ErrorCodeNone;
}

void websocket_loop(PWebSocketInfo websocket)
{
    if (websocket == NULL ||
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
    if (websocket == NULL ||
        websocket->impl == NULL ||
        websocket->impl->context == NULL) {
        return;
    }

    lwsl_user("deinit...\n");
    lws_context_destroy(websocket->impl->context);
    free(websocket->impl);
    websocket->impl = NULL;
}

void websocket_printf(const char* format, ...)
{
    va_list args;
    char    buffer[4096];

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    lwsl_user("%s", buffer);
    va_end(args);
}
