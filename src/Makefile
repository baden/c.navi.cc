
CFLAGS = -O3

# Библиотеки
LIB_EVHTP_INCLUDES = -I ../deps/libevhtp -I ../deps/libevhtp/build -I ../deps/libevhtp/evthr -I ../deps/libevhtp/htparse -I ../deps/libevhtp/oniguruma
LIB_EVHTP_LIB = ../deps/libevhtp/build/libevhtp.a

LIB_WEBSOCK_INCLUDES = -I ../deps/libwebsock/src
LIB_WEBSOCK_LIB = -L ../deps/libwebsock/src/.libs/ -lwebsock

INCLUDES = $(LIB_EVHTP_INCLUDES) $(LIB_WEBSOCK_INCLUDES)

all: cnavicc

cnavicc: http-server.c
	gcc -o cnavicc http-server.c $(CFLAGS) $(INCLUDES) $(LIB_EVHTP_LIB) -levent -levent_pthreads -lpthread


echo-server: echo.c
	# libtool --mode=link gcc -o echo echo.c $(CFLAGS) $(INCLUDES) $(LIB_WEBSOCK_LIB) ../deps/libwebsock/src/.libs/libwebsock.la
	libtool --mode=link gcc -o echo-server echo.c $(CFLAGS) $(INCLUDES) ../deps/libwebsock/src/.libs/libwebsock.la

