/**
 * @file  websockets.h
 * @brief [description]
 */

#ifndef _SHOOTING_STR_WEB_SOCKET_H_
#define _SHOOTING_STR_WEB_SOCKET_H_

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../utils/log.h"

/*----------------------------------------------------------------------------*/
/* Types                                                                      */
/*----------------------------------------------------------------------------*/

/**
 * @brief web socket error code
 */
enum WEB_SOCKET_ERROR_CODE {
    ErrorCodeNone = 0,
    ErrorCodeInvalidArgument,
    ErrorCodeInitializeSocket
};

/**
 * @brief Websocket callback.
 *
 * @param [in]  user             [description]
 * @param [in]  received_data    [description]
 */
typedef int (*PWebSocketReceiveCallback)(
    void*       user,
    const char* received_data);

/*----------------------------------------------------------------------------*/
/* Structs                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * @brief websocket info
 */
typedef struct _WebSocketInfoImpl WebSocketInfoImpl, *PWebSocketInfoImpl;

/**
 * @brief websocket info
 */
typedef struct _WebSocketInfo {
    int                       port;
    int                       uid;
    int                       gid;
    PWebSocketReceiveCallback callback;
    PWebSocketInfoImpl        impl;
} WebSocketInfo, *PWebSocketInfo;

/*----------------------------------------------------------------------------*/
/* Prototype functions                                                        */
/*----------------------------------------------------------------------------*/

/**
 * @brief websocket init
 *
 * @param [in] websocket [description]
 *
 * @return [description]
 * @retval value [return value description]
 */
bool websocket_init(PWebSocketInfo websocket);

/**
 * @brief websocket loop
 *
 * @param [in] websocket [description]
 */
bool websocket_loop(PWebSocketInfo websocket);

/**
 * @brief websocket deinit
 *
 * @param [in] websocket [description]
 */
bool websocket_deinit(PWebSocketInfo websocket);

void websocket_setsignal();

bool websocket_write(const char* buf, const size_t len);

void websocket_logdump_callback(const enum LogKind kind, const char* str);

#define websocket_logdump(kind, ...) \
    logdump(kind, 4096, websocket_logdump_callback, __FILE__, __LINE__, __VA_ARGS__)

#endif
