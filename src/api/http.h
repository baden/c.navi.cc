#ifndef H_API_HTTP
#define H_API_HTTP


evhtp_t * api_init(evbase_t * evbase);
void api_done(evhtp_t  * htpAPI);


#endif

