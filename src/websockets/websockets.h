/**
 * @file  websockets.h
 * @brief [description]
 */

#ifndef _SHOOTING_STR_WEB_SOCKET_H_
#define _SHOOTING_STR_WEB_SOCKET_H_

/*----------------------------------------------------------------------------*/
/* Headers                                                                    */
/*----------------------------------------------------------------------------*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

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
 * @param [in]  data             [description]
 * @param [in]  write_buffer_len [description]
 * @param [out] write_buffer     [description]
 */
typedef int (*WebSocketCallback)(
    void*          user,
    const char*    data,
    const int      write_buffer_len,
    unsigned char* write_buffer);

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
    int                port;
    int                uid;
    int                gid;
    WebSocketCallback  callback;
    PWebSocketInfoImpl impl;
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
enum WEB_SOCKET_ERROR_CODE websocket_init(PWebSocketInfo websocket);

/**
 * @brief websocket loop
 *
 * @param [in] websocket [description]
 */
void websocket_loop(PWebSocketInfo websocket);

/**
 * @brief websocket deinit
 *
 * @param [in] websocket [description]
 */
void websocket_deinit(PWebSocketInfo websocket);

/**
 * @brief print
 *
 * @param [in] format [description]
 */
void websocket_printf(const char* format, ...);

#endif
