#ifndef __TWS_PRIVATE_H__
#define __TWS_PRIVATE_H__

#ifndef __TWS_INTERNAL__
#error "don't include tws private header"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <uv.h>
#include <pt.h>
#include <twsclient/tws_client.h>

#define CLIENT_VERSION          63

#define MIN_SERVER_VER_LINKING  70
#define MIN_REQ_SERVER_VERSION  MIN_SERVER_VER_LINKING

#define IS_EMPTY(str)  (!(str) || ((str)[0] == '\0'))

typedef struct real_client_s {
    TWS_CLIENT_PUBLIC_FIELDS

    /* follow fileds are private fields */

    bool free_flag;

    /* uv */
    uv_loop_t *loop;
    uv_tcp_t socket;
    uv_connect_t conn_req;
    struct sockaddr_in req_addr;

    /* reconnect support */
    uv_timer_t reconnect_timer;
    int enable_reconnect;
    int reconnects;
    int reconnecting;
    int max_reconnects_incr;
    int reconnect_delay;
    int reconnect_delay_max;
    unsigned seed;

    /* write buf */
    char wbuf[1024];
    size_t wlen;

    /* read buf */
    char rbuf[4096];
    size_t rstart;
    size_t rlen;

    /* event */
    tws_event_callback event_cb;
    tws_event_t event;

    /* package decoder */
    struct pt pt;   /* coroutine status */
    char *p;        /* current parser position */
    int cmd;        /* current decoding cmd */
}real_client_t;

typedef void (*tws_client_generical_cb)(real_client_t *client);

/* tws_utils export */
#ifndef NDEBUG
void hexdump(char *prefix, void *ptr, int buflen);
#endif

#ifdef USE_JEMALLOC
char *sk_strdup(const char *src);
#endif

/* tws_client export */
void tws_client_write(real_client_t *client, tws_client_generical_cb callback);

/* tws_encode export */
void tws_client_logon(real_client_t *client);
void tws_client_startAPI(real_client_t *client);

/* tws_decode export */
void tws_decode_reset(real_client_t *client);
void tws_decode_message(real_client_t *client, char *token);

#define LOG_ERR(...) \
    if (client->logger) \
        client->logger(LEVEL_ERROR, __VA_ARGS__)

#define LOG_INFO(...) \
    if (client->logger) \
        client->logger(LEVEL_INFO, __VA_ARGS__)

#define LOG_DEBUG(...) \
    if (client->logger) \
        client->logger(LEVEL_DEBUG, __VA_ARGS__)

#define LOG_WARN(...) \
    if (client->logger) \
        client->logger(LEVEL_WARN, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // __TWS_PRIVATE_H__
