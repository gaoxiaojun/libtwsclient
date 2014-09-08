#include <uv.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef USE_ZLOG
#include <zlog.h>
#endif

#include <twsclient/tws_client.h>

extern void tws_model_event_cb(tws_client_t *client, int event_type, void *ud);
extern void tws_debug(tws_client_t *client, int event_type, void *ud);
extern void* tws_model_new_context(tws_client_t *client);

void sig_int_cb(uv_signal_t *handle, int signum)
{
    uv_signal_stop(handle);
    uv_stop(uv_default_loop());
}

#ifdef USE_ZLOG
static int mk_rules_file(char *rule_file)
{
    FILE *fp = NULL;

    fp = fopen(rule_file, "w+");

    if(NULL == fp) {
        printf("fopen error: %s\n", strerror(errno));
        return -1;
    }

    fprintf(fp, \
            "[global]\n"\
            "default format = \"%%D.%%ms %%V [%%f:%%U:%%L] %%m%%n\"\n"\
            "[levels]\n"\
            "INFO = 40, LOG_INFO\n"\
            "[rules]\n"\
            "*.* >stdout\n" \
            );
    fclose(fp);
    return 0;
}
#endif

int main(int argc, char **argv)
{
    int err;
    unsigned int port = 7496;
    const char *host = "127.0.0.1";

#ifdef USE_ZLOG
    char *conf_path = "log.conf";
    char *cat = "default";

    err = dzlog_init(conf_path, cat);

    if(err) {
        mk_rules_file(conf_path);

        if (dzlog_init(conf_path, cat))
            return -1;
    }
#endif

    if(argc  == 3) {
        host = argv[1];
        port = atoi(argv[2]);
    } else if (argc == 2) {
        port = atoi(argv[1]);
    }

    uv_loop_t *loop = uv_default_loop();
    if (!loop)
        return ENOMEM;

    uv_signal_t sig_int;
    uv_signal_init(loop, &sig_int);
    uv_signal_start(&sig_int, sig_int_cb, SIGINT);

#ifdef PLATFORM_POSIX
    signal(SIGPIPE, SIG_IGN);
#endif

    tws_client_t *client = tws_client_new_with_reconnect(loop, tws_debug,
                                                         2, 10, 0);

    if (!client)
        return ENOMEM;

    err = tws_client_connect(client, host, port);
    if (err)
        return err;

    uv_run(loop, UV_RUN_DEFAULT);

    tws_client_destroy(client);

#ifdef USE_ZLOG
    zlog_fini();
#endif

    return 0;
}
