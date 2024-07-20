/**
 * @file main.c
 * @brief entry point
 */

#include "websockets/websockets.h"
#include "utils/compatibility.h"
#include <yyjson.h>

int user_callback(
    void*       user,
    const char* data,
    const int   max_write_buffer_len,
    char*       write_buffer)
{
    websocket_printf("user_callback data : [%s]", data);

    size_t      len = strlen(data);
    yyjson_doc* doc = yyjson_read(data, len, 0);
    if (doc == NULL) {
        websocket_printf("yyjson_read failed.\n");
        goto FINALIZE;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        websocket_printf("yyjson_get_root failed.\n");
        goto FINALIZE;
    }

    yyjson_val* event_type_obj = yyjson_arr_get(root, 0);
    if (!yyjson_is_str(event_type_obj)) {
        websocket_printf("event type is not str.\n");
        goto FINALIZE;
    }

    yyjson_val* sub_id_obj = yyjson_arr_get(root, 1);
    if (!yyjson_is_str(sub_id_obj)) {
        websocket_printf("sub_id is not str.\n");
        goto FINALIZE;
    }

    const char* event_type = yyjson_get_str(event_type_obj);
    const char* sub_id     = yyjson_get_str(sub_id_obj);

    if (strstr(event_type, "EVENT")) {
        yyjson_val* event_data = yyjson_arr_get(root, 2);
        if (!yyjson_is_obj(event_data)) {
            websocket_printf("EVENT data is not json\n");
            goto FINALIZE;
        }

        // Send OK message
        snprintf(write_buffer, max_write_buffer_len, "[\"OK\",\"%s\",true,\"\"]", sub_id);
        goto FINALIZE;
    } else if (strstr(event_type, "CLOSE")) {
        // Send EOSE message
        snprintf(write_buffer, max_write_buffer_len, "[\"EOSE\",\"%s\"]", sub_id);
        goto FINALIZE;
    }

FINALIZE:
    if (doc != NULL) {
        yyjson_doc_free(doc);
    }
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
