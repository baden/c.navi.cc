#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <evhtp.h>

#define POINT_PORT 8781

void testcb(evhtp_request_t * req, void * a) {
    const char * str = a;

    printf("Call testcb\n");

    evbuffer_add_printf(req->buffer_out, "%s", str);
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void info_cb(evhtp_request_t * req, void * a) {
    const char * str = a;

    // printf("Call info_cb\n");

    // evbuffer_add_printf(req->buffer_out, "%s", str);
    evbuffer_add_printf(req->buffer_out, "REST methods: GET");
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
    evhtp_use_threads(htp, NULL, 4, NULL);
#endif
    evhtp_bind_socket(htp, "0.0.0.0", POINT_PORT, 1024);

    event_base_loop(evbase, 0);

    evhtp_unbind_socket(htp);
    evhtp_free(htp);
    event_base_free(evbase);

    return 0;
}

