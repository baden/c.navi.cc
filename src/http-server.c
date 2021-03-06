#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <evhtp.h>

#include "global.h"

#include "api/http.h"

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


void testcb(evhtp_request_t * req, void * a) {
    const char * str = a;

    printf("Call testcb\n");

    evbuffer_add_printf(req->buffer_out, "%s", str);
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void info_cb(evhtp_request_t * req, void * a) {
    // const char * str = a;
    evhtp_headers_t * headers;

    if (!(headers = req->headers_out)) {
        printf("Error set headers\n");
        return;
    }

    int req_method = evhtp_request_get_method(req);
    const char *uri = req->uri->path->full;

    evhtp_headers_add_header(headers, evhtp_header_new("Access-Control-Allow-Credentials", "true", 0, 0));
    evhtp_headers_add_header(headers, evhtp_header_new("Access-Control-Allow-Origin", "*", 0, 0));
    evhtp_headers_add_header(headers, evhtp_header_new("Server", SERVER_NAME, 0, 0));
    evhtp_headers_add_header(headers, evhtp_header_new("Content-Type", "text/plain", 0, 0));
    evhtp_headers_add_header(headers, evhtp_header_new("X-Navi-API-version", "1.0", 0, 0));

    // evbuffer_add_printf(req->buffer_out, "%s", str);
    evbuffer_add_printf(req->buffer_out, "POINT methods: GET POST\r\n");
    evbuffer_add_printf(req->buffer_out, "uri : %s\r\n", uri);
    evbuffer_add_printf(req->buffer_out, "query : %s\r\n", req->uri->query_raw);
    evbuffer_add_printf(req->buffer_out, "Method : %s\n", method_strmap[req_method]);

    evbuffer_add_printf(req->buffer_out, "PID : %ld\r\n", _evhtp_ssl_get_thread_id());
    evhtp_send_reply(req, EVHTP_RES_OK);
}


int main(int argc, char ** argv) {

    printf("Starting server at 0.0.0.0:8781\n");

    evbase_t * evbase = event_base_new();

    evhtp_t  * htp    = evhtp_new(evbase, NULL);

    evhtp_set_cb(htp, "/info", info_cb, "info");
    evhtp_set_cb(htp, "/1/ping", testcb, "one");
    evhtp_set_cb(htp, "/1/ping.json", testcb, "two");
#ifndef EVHTP_DISABLE_EVTHR
    evhtp_use_threads(htp, NULL, 4, NULL);      // Задействовать четыре процесса (ядра?)
#endif
    evhtp_bind_socket(htp, "0.0.0.0", POINT_PORT, 1024);


    evhtp_t  * htpAPI = api_init(evbase);


    event_base_loop(evbase, 0);

    api_done(htpAPI);

    evhtp_unbind_socket(htp);
    evhtp_free(htp);

    event_base_free(evbase);

    return 0;
}

