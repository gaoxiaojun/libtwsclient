#include <uv.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <twsclient/tws_client.h>

extern void tws_debug(tws_client_t *client, int event_type, void *ud);

static void debug_printf(FILE *stream, const char *label, const char *fmt, va_list ap)
{
  char fmtbuf[1024];
  vsnprintf(fmtbuf, sizeof(fmtbuf), fmt, ap);
  fprintf(stream, "%s: %s\n",label, fmtbuf);
}

void debug_logger(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  debug_printf(stderr, "error", fmt, ap);
  va_end(ap);
}

/*
void sig_int_cb(uv_signal_t *handle, int signum)
{
    printf("user interrupt\n");
    uv_signal_stop(handle);
    uv_stop(uv_default_loop());
}
*/

int main(int argc, char **argv)
{
    int err;
    unsigned int port = 7496;
    const char *host = "127.0.0.1";

    if(argc  == 3) {
        host = argv[1];
        port = atoi(argv[2]);
    } else if (argc == 2) {
        port = atoi(argv[1]);
    }

    uv_loop_t *loop = uv_default_loop();
    if (!loop)
        return ENOMEM;

    /*
    uv_signal_t sig_int;
    uv_signal_init(loop, &sig_int);
    uv_signal_start(&sig_int, sig_int_cb, SIGINT);
    */

#ifdef PLATFORM_POSIX
    signal(SIGPIPE, SIG_IGN);
#endif

    tws_client_t *client = tws_client_new_with_reconnect(loop, tws_debug,
                                                         2, 10, 0);

    if (!client)
        return ENOMEM;

    client->logger = debug_logger;

    err = tws_client_connect(client, host, port);
    if (err)
        return err;

    uv_run(loop, UV_RUN_DEFAULT);

    tws_client_destroy(client);

    printf("exit...\n");
    return 0;
}
