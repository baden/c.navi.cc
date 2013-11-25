// #include "request.h"
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVERPORT 8080
#define WEBROOT "/home/yilang/www/"

void general_request_cb(struct evhttp_request *req, void *arg)
{
    struct evbuffer *evb = evbuffer_new();

    evbuffer_add_printf(evb, "%s", "<h1>It works!\n</h1>");

    // list_support_request(evb);

    evhttp_send_reply(req, 200 , "ok", evb);

}

const char* get_request_host(struct evhttp_request *req)
{
    return evhttp_request_get_host(req);
}

// void add_headers_to_evb(struct evbuffer *evb, struct evkeyvalq *headers)
// {
//     struct evkeyval *header;

//     evbuffer_add_printf(evb, "<ol>headers");

//     for(header = headers->tqh_first; header; header = header->next.tqe_next){
//         evbuffer_add_printf(evb, "<li>key = %s, value = %s</li>", header->key, header->value);
//     }

//     evbuffer_add_printf(evb, "</ol>");

// }

struct evkeyvalq* get_request_input_headers(struct evhttp_request *req)
{
    struct evkeyvalq * input_headers;
    input_headers = evhttp_request_get_input_headers(req);

    return input_headers;

}

void html_request_cb(struct evhttp_request *req, void *arg)
{
    struct evbuffer *evb = evbuffer_new();

    int fd;
    char *url ;

    strcpy(url, WEBROOT);
    strcat(url, "index.html");

    if((fd = open(url, O_RDONLY)) < 0){
        perror("open");
        return ;
    }

    struct stat st;

    if((fstat(fd, &st)) < 0){
        perror("fstat");
        return ;
    }

    evbuffer_add_printf(evb, "%s", "<h1><center>fake baidu for html request!</center></h1>\n");

    evbuffer_add_printf(evb, "host:%s", get_request_host(req));

    if((evbuffer_add_file(evb, fd, 0, st.st_size)) < 0){
        perror("evbuffer_add_file");
        return ;
    }
    evhttp_send_reply(req, 200, "ok", evb);

}

void show_request_cb(struct evhttp_request *req, void *arg)
{
    struct evbuffer *evb = evbuffer_new();

    evbuffer_add_printf(evb, "<h3>%s</h3>", "Below shows the request info:");
    /* evbuffer_add_printf(evb, "<p>request cmd type:%s</p>",  get_request_method(req)); */
    /* evbuffer_add_printf(evb, "<p>request from host:%s </p>  ", get_request_host(req)); */
    // add_headers_to_evb(evb, get_request_input_headers(req));

    evhttp_send_reply(req, 200, "ok", evb);
}


void list_support_request(void *arg)
{
    struct evbuffer *evb = (struct evbuffer *)arg;

    evbuffer_add_printf(evb, "%s", "<ul><h3>list support request:</h3>");

    evbuffer_add_printf(evb, "%s", "<li><a href=\"/request\">request</a></li>");
    evbuffer_add_printf(evb, "%s", "<li><a href=\"/html\">baidu</a></li>");

    evbuffer_add_printf(evb, "%s", "</ul>");

}

int main(int argc, char **argv)
{
    struct event_base *base;
    struct evhttp *evhttp;
    struct evhttp_bound_socket *handle;

    base = event_base_new();

    evhttp = evhttp_new(base);

    evhttp_set_gencb(evhttp, general_request_cb, NULL);

    evhttp_set_cb(evhttp, "/html", html_request_cb, NULL);

    evhttp_set_cb(evhttp, "/baidu", html_request_cb, NULL);

    evhttp_set_cb(evhttp, "/request", show_request_cb, NULL);

    handle = evhttp_bind_socket_with_handle(evhttp, "0.0.0.0", SERVERPORT);

    event_base_dispatch(base);

    return 0;
}
