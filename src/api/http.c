#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <evhtp.h>

#include "../global.h"

#include "http.h"

struct app_parent {
    evhtp_t  * evhtp;
    evbase_t * evbase;
    char     * redis_host;
    uint16_t   redis_port;
};

struct app {
    struct app_parent * parent;
    evbase_t          * evbase;

    struct event      * ev_timer;
    // redisAsyncContext * redis;
    long int counter;
    long int garb_counter;
};

static evthr_t *
get_request_thr(evhtp_request_t * request) {
    evhtp_connection_t * htpconn;
    evthr_t            * thread;

    htpconn = evhtp_request_get_connection(request);
    thread  = htpconn->thread;

    return thread;
}


static const char * method_strmap[] = {
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "MKCOL",
    "COPY",
    "MOVE",
    "OPTIONS",
    "PROPFIND",
    "PROPATCH",
    "LOCK",
    "UNLOCK",
    "TRACE",
    "CONNECT",
    "PATCH",
    "UNKNOWN",
};


static unsigned long
_evhtp_ssl_get_thread_id(void) {
#ifndef WIN32
    return (unsigned long)pthread_self();
#else
    return (unsigned long)(pthread_self().p);
#endif
}

#define GARB_TIMEOUT 2


void api_info_cb(evhtp_request_t * req, void * a) {

    // struct app_parent  * app_parent;
    // struct app         * app;
    // evthr_t            * thread;
    // evhtp_connection_t * conn;
    // struct sockaddr_in * sin;

    // printf("process_request = %p\n", req);

    evthr_t            * thread = get_request_thr(req);

    // printf("thread = %p\n", thread);

    // conn   = evhtp_request_get_connection(req);
    // printf("conn = %p\n", conn);

    struct app         * app    = (struct app *)evthr_get_aux(thread);
    // printf("app = %p\n", app);
    // printf("counter = %ld\n", app->counter);
    app->counter++;

    // sin    = (struct sockaddr_in *)conn->saddr;
    // printf("sin = %p\n", sin);



    // const char * str = a;
    evhtp_headers_t * headers;

    if (!(headers = req->headers_out)) {
        printf("Error set headers\n");
        return;
    }

    int req_method = evhtp_request_get_method(req);
    const char *uri = req->uri->path->full;

    evhtp_headers_add_header(headers, evhtp_header_new("Server", SERVER_NAME, 0, 0));
    evhtp_headers_add_header(headers, evhtp_header_new("Content-Type", "text/plain", 0, 0));
    evhtp_headers_add_header(headers, evhtp_header_new("Connection", "keep-alive", 0, 0));
    evhtp_headers_add_header(headers, evhtp_header_new("Access-Control-Allow-Credentials", "true", 0, 0));
    evhtp_headers_add_header(headers, evhtp_header_new("Access-Control-Allow-Origin", "*", 0, 0));
    evhtp_headers_add_header(headers, evhtp_header_new("X-Navi-API-version", "1.0", 0, 0));

    // evbuffer_add_printf(req->buffer_out, "%s", str);
    evbuffer_add_printf(req->buffer_out, "API methods: GET POST PUT DELETE\r\n");
    evbuffer_add_printf(req->buffer_out, "uri : %s\r\n", uri);
    evbuffer_add_printf(req->buffer_out, "query : %s\r\n", req->uri->query_raw);
    evbuffer_add_printf(req->buffer_out, "Method : %s\n", method_strmap[req_method]);

    evbuffer_add_printf(req->buffer_out, "PID : %lX\r\n", _evhtp_ssl_get_thread_id());
    evbuffer_add_printf(req->buffer_out, "COUNTER : %ld\r\n", app->counter);
    evbuffer_add_printf(req->buffer_out, "GARB COUNTER : %ld\r\n", app->garb_counter);
    evhtp_send_reply(req, EVHTP_RES_OK);

    struct timeval http_timer_tv;
    http_timer_tv.tv_sec = GARB_TIMEOUT;
    http_timer_tv.tv_usec = 0;
    event_add(app->ev_timer, &http_timer_tv);
}


static void http_timer(evutil_socket_t fd, short events, void *arg)
{
    struct app        * app;
    struct timeval http_timer_tv;

    http_timer_tv.tv_sec = GARB_TIMEOUT;
    http_timer_tv.tv_usec = 0;

    //hlog(LOG_DEBUG, "http_timer fired");

    // if (http_shutting_down || http_reconfiguring) {
    //         http_timer_tv.tv_usec = 1000;
    //         event_base_loopexit(libbase, &http_timer_tv);
    //         return;
    // }

    app  = (struct app *)arg;

    // printf("Timer fire %ld\n", app->counter);
    app->garb_counter++;

    // const char *data = arg;
    // printf("Got an event on socket %d:%s%s%s%s",
    //     (int) fd,
    //     (events & EV_TIMEOUT) ? " timeout" : "",
    //     (events & EV_READ)    ? " read" : "",
    //     (events & EV_WRITE)   ? " write" : "",
    //     (events & EV_SIGNAL)  ? " signal" : ""
    // );
    // Для оттягивания события можно пользоваться:
    event_add(app->ev_timer, &http_timer_tv);

    // Для удаления (отмены?):
    // event_del(app->ev_timer);

    // Для удаления события (очистки памяти):
    // event_free(app->ev_timer);
}


void
app_init_thread(evhtp_t * htp, evthr_t * thread, void * arg) {
    struct app_parent * app_parent;
    struct app        * app;

    app_parent  = (struct app_parent *)arg;
    app         = calloc(sizeof(struct app), 1);

    app->parent = app_parent;
    app->evbase = evthr_get_base(thread);
    app->counter = 0;
    app->garb_counter = 0;
    // app->redis  = redisAsyncConnect(app_parent->redis_host, app_parent->redis_port);

    // redisLibeventAttach(app->redis, app->evbase);

    printf("API thread %lX\n", _evhtp_ssl_get_thread_id());
    // evbuffer_add_printf(req->buffer_out, "PID : %ld\r\n", );

    struct timeval http_timer_tv;

    http_timer_tv.tv_sec = GARB_TIMEOUT;
    http_timer_tv.tv_usec = 0;

    // ev_timer = NULL;

    // printf("Test timer\n");
    app->ev_timer = event_new(app->evbase, -1, EV_TIMEOUT, http_timer, app);
    event_add(app->ev_timer, &http_timer_tv);

    evthr_set_aux(thread, app);
}



evhtp_t * api_init(evbase_t * evbase) {
    struct app_parent * app_p;

    evhtp_t  * evhtp    = evhtp_new(evbase, NULL);

    app_p             = calloc(sizeof(struct app_parent), 1);
    app_p->evhtp      = evhtp;
    app_p->evbase     = evbase;
    app_p->redis_host = "127.0.0.1";
    app_p->redis_port = 6379;



    evhtp_set_cb(evhtp, "/info", api_info_cb, "info");
    // evhtp_set_cb(htpAPI, "/1/ping", testcb, "one");
    // evhtp_set_cb(htp, "/1/ping.json", testcb, "two");
#ifndef EVHTP_DISABLE_EVTHR
    // evhtp_use_threads(evhtp, NULL, 4, NULL);   // Задействовать четыре процесса (плюс к тем что у htp)
                                                // Итого будет задействовано 9 потоков
    evhtp_use_threads(evhtp, app_init_thread, 4, app_p);
#endif
    evhtp_bind_socket(evhtp, "0.0.0.0", API_PORT, 1024);

    return evhtp;
}


void api_done(evhtp_t * evhtp) {
    evhtp_unbind_socket(evhtp);
    evhtp_free(evhtp);
}
