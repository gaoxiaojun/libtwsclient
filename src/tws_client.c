#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <assert.h>

#include <twsclient/tws_client.h>

#define __TWS_INTERNAL__
#include "tws_private.h"


#define CONTAINER_OF(ptr, type, field)                                        \
  ((type *) ((char *) (ptr) - ((char *) &((type *) 0)->field)))

#define MAX_REQUEST_MESSAGE_SIZE 1024

/* forward internal API declare */
static int _client_connect(real_client_t *client);
static void _client_reconnect_start(real_client_t *client);

/* libuv callbacks */
void on_write_cb(uv_write_t* req, int status)
{
    real_client_t *client = req->data;
    tws_client_generical_cb *cb = (tws_client_generical_cb *)((uv_buf_t *)(req + 1) + 1);

    sk_free(req);
    if (status < 0) {
        if (client->logger)
            client->logger("Error[%d]:%s\n", status, uv_strerror(status));
        tws_client_close((tws_client_t *)client);
        return;
    }
    if (*cb != NULL) {
        (*cb)(client);
    }
}

static void on_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    real_client_t *client = CONTAINER_OF(handle, real_client_t, socket);

    int rbuf_free_len = sizeof(client->rbuf) - (client->rstart + client->rlen);

    if (rbuf_free_len < MAX_REQUEST_MESSAGE_SIZE) {
            memmove(client->rbuf, client->rbuf +client->rstart, client->rlen);
            client->p -= client->rstart;
            client->rstart = 0;
            rbuf_free_len = sizeof(client->rbuf) - (client->rstart + client->rlen);
    }

    buf->base = client->rbuf + client->rstart + client->rlen;
    buf->len =  sizeof(client->rbuf) - (client->rstart + client->rlen);
}

static void on_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    real_client_t *client = CONTAINER_OF(stream, real_client_t, socket);

    if (nread < 0) {
        if (client->logger)
            client->logger("Error[%zd]:%s\n", nread, uv_strerror(nread));
        tws_client_close((tws_client_t *)client);
        return;
    }
#ifndef NDEBUG
    hexdump("->  ", buf->base, nread);
#endif

    client->rlen += nread;

    while (client->p < client->rbuf + client->rstart + client->rlen) {
        int token_found = 0;

        while (client->p < client->rbuf + client->rstart + client->rlen) {
            if (*(client->p++) == '\0') {
                token_found = 1;
                break;
            }
        }

        if (!token_found) return;

#ifndef NDEBUG
        hexdump("-TT-", client->rbuf + client->rstart, client->p - client->rbuf - client->rstart);
#endif
        tws_decode_message(client, client->rbuf + client->rstart);

        int delta = client->p - (client->rbuf + client->rstart);
        client->rlen -= delta;
        client->rstart += delta;
    }
}

static void on_connect(uv_connect_t *req, int status)
{
    real_client_t *client = CONTAINER_OF(req, real_client_t, conn_req);

    if (status != 0) {
        if (client->logger)
            client->logger("Error[%d]:%s\n", status, uv_strerror(status));
        tws_client_close((tws_client_t *)client);
        return;
    }

    client->state = TWS_ST_CONNECTED;

    if (client->reconnecting)
        uv_timer_stop(&client->reconnect_timer);

    uv_read_start(req->handle, on_alloc_cb, on_read_cb);

    tws_client_logon(client);
}

static void on_reconnect_timer_cb(uv_timer_t* timer)
{
  real_client_t* client = (real_client_t*)timer->data;

  if (client->state < TWS_ST_CONNECTING) {
      client->clientId++;
      _client_connect(client);
  }
}

static void on_close_cb(uv_handle_t* handle)
{
    real_client_t *client = CONTAINER_OF(handle, real_client_t, socket);

    client->state = TWS_ST_CLOSED;

    if (client->free_flag) {
        sk_free(client);
        return;
    }

    _client_reconnect_start(client);
}

/* libuv helper */
static int _client_connect(real_client_t *client)
{
    tws_decode_reset(client);

    client->state = TWS_ST_CONNECTING;

    uv_tcp_init(client->loop, &client->socket);
    uv_tcp_nodelay(&client->socket, 1);

    int err = uv_tcp_connect(&client->conn_req, &client->socket,
                         (const struct sockaddr*)&client->req_addr, on_connect);
    if (err < 0) {
        if (client->logger)
            client->logger("Error[%d]:%s\n", err, uv_strerror(err));
        client->state = TWS_ST_CLOSED;
        _client_reconnect_start(client);
        return err;
    }

    return 0;
}

static void _client_reconnect_start(real_client_t *client)
{
    if (client->enable_reconnect) {
        client->reconnect_timer.data = client;
        client->rstart = 0;
        client->rlen = 0;
        client->reconnects++;
        int delay = 0;
        if (client->reconnects >= client->max_reconnects_incr) {
            delay = client->reconnect_delay_max;
        } else {
          delay = client->reconnect_delay * client->reconnects;
      }

      if (delay > client->reconnect_delay_max) delay = client->reconnect_delay_max;

      delay = rand_r(&client->seed) % delay + delay;

      if (client->logger)
          client->logger("reconnect: %d, delay: %d\n", client->reconnects, delay);
      uv_timer_start(&client->reconnect_timer, on_reconnect_timer_cb, delay * 1000, 0);
    }
}

/* Public API */

tws_client_t * tws_client_new(uv_loop_t *loop, tws_event_callback cb)
{
    real_client_t *client = sk_calloc(sizeof(struct real_client_s), 1);

    if (client) {
        client->serverVersion = 0;
        client->clientId = 0;

        client->loop = loop;

        memset(client->wbuf, 0, sizeof(client->wbuf));
        client->wlen = 0;

        memset(client->rbuf, 0, sizeof(client->rbuf));
        client->rstart = 0;
        client->rlen = 0;

        client->event_cb = cb;

        client->logger = NULL;

        client->state = TWS_ST_CLOSED;
    }

    return (tws_client_t *)client;
}

int tws_client_enable_reconnect(tws_client_t *c, int delay, int delay_max)
{
    real_client_t *client = (real_client_t *)c;

    if (delay <=0  || delay_max <= 0)
        return EINVAL;

    client->enable_reconnect = 1;
    client->reconnect_delay = delay;
    client->reconnect_delay_max = delay_max;
     client->seed = time(0);
    client->max_reconnects_incr = client->reconnect_delay_max / client->reconnect_delay + 1;

    uv_timer_init(client->loop, &client->reconnect_timer);

    return 0;
}

void tws_client_disable_reconnect(tws_client_t *c)
{
    real_client_t *client = (real_client_t *)c;
    client->enable_reconnect = 0;
    if (uv_is_active((uv_handle_t *)&client->reconnect_timer))
        uv_timer_stop(&client->reconnect_timer);
}

void tws_client_destroy(tws_client_t *c)
{
    real_client_t *client = (real_client_t *)c;

     client->free_flag = 1;
     if (TWS_ST_CLOSED != client->state) {
         tws_client_stop((tws_client_t *)client);
     } else {
         sk_free(client);
     }
}

void tws_client_stop(tws_client_t *c)
{
    real_client_t *client = (real_client_t *)c;

    client->enable_reconnect = 0;

    if (uv_is_active((uv_handle_t *)&client->reconnect_timer))
        uv_timer_stop(&client->reconnect_timer);

    if (TWS_ST_CLOSED != client->state) {
      tws_client_close((tws_client_t *)client);
    }
}

int tws_client_connect(tws_client_t *c, const char *host, unsigned int port)
{
    real_client_t *client = (real_client_t *)c;

    int err;

    err = uv_ip4_addr(host, port, &client->req_addr);
    if (err < 0) {
        if (client->logger)
            client->logger("Error[%d]:%s\n", err, uv_strerror(err));
        /* if address failed, don't reconnect again */
        client->enable_reconnect = 0;
        return err;
    }

    return (_client_connect(client));
}

void tws_client_close(tws_client_t *c)
{
    real_client_t *client = (real_client_t *)c;

    if (client->state > TWS_ST_DISCONNECTING) {
        client->state = TWS_ST_DISCONNECTING;

        if (client->event_cb != NULL)
            client->event_cb((tws_client_t *)client, CLIENT_DISCONNECTED, NULL);

        uv_close((uv_handle_t *)&client->socket, on_close_cb);
    }
}

void tws_client_write(real_client_t *client, tws_client_generical_cb callback)
{
    uv_write_t *write_req = (uv_write_t *)sk_malloc(sizeof(uv_write_t) +
                                                 sizeof(uv_buf_t) +
                                                 sizeof(tws_client_generical_cb) +
                                                 client->wlen);
    write_req->data = client;

    uv_buf_t *buf = (uv_buf_t *) ((char *)write_req + sizeof(uv_write_t));
    tws_client_generical_cb *cb = (tws_client_generical_cb *)((uv_buf_t *)(write_req + 1) + 1);
    char *content = (char *) ((char *)write_req + sizeof(uv_write_t) + sizeof(uv_buf_t) +
                              sizeof(tws_client_generical_cb));
    buf->base = content;
    buf->len = client->wlen;

    *cb = callback;
    memcpy(buf->base, client->wbuf, client->wlen);
#ifndef NDEBUG
    hexdump("<-  ", buf->base, client->wlen);
#endif
    uv_write(write_req, (uv_stream_t *)&client->socket, buf, 1, on_write_cb);
    client->wlen = 0;
}
