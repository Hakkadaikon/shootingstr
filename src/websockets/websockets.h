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

/*----------------------------------------------------------------------------*/
/* Enums                                                                      */
/*----------------------------------------------------------------------------*/

/**
 * @brief error code
 */
enum WEB_SOCKET_ERROR_CODE {
    ErrorCodeNone = 0,
    ErrorCodeInvalidArgument,
    ErrorCodeInitializeSocket
};

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

#endif
