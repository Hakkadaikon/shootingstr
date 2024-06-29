#include <libwebsockets.h>
#include <yyjson.h>
#include <string.h>
#include <stdio.h>

static int callback_websockets(
    struct lws* wsi,
    enum lws_callback_reasons reason,
    void* user,
    void* in,
    size_t len
)
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

static struct lws_protocols protocols[] = {
    {
        "websocket-protocol",
        callback_websockets,
        0,
        4096,
    },
    {NULL, NULL, 0, 0} /* terminator */
};

int main(int argc, char** argv)
{
    struct lws_context_creation_info info;
    struct lws_context*              context;

    memset(&info, 0, sizeof(info));
    info.port      = 8080;
    info.protocols = protocols;
    info.gid       = -1;
    info.uid       = -1;

    context = lws_create_context(&info);
    if (context == NULL) {
        lwsl_err("lws_create_context failed\n");
        return -1;
    }

    lwsl_user("Starting server...\n");
    while (1) {
        lws_service(context, 1000);
    }

    lws_context_destroy(context);
    return 0;
}
