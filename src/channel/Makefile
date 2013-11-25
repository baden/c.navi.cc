
CFLAGS = -O3 -g

DEPS = ../../deps

# Библиотеки
LIB_EVHTP_INCLUDES = -I $(DEPS)/libevhtp -I $(DEPS)/libevhtp/build -I $(DEPS)/libevhtp/evthr -I $(DEPS)/libevhtp/htparse -I $(DEPS)/libevhtp/oniguruma
LIB_EVHTP_LIB = $(DEPS)/libevhtp/build/libevhtp.a

LIB_WEBSOCK_INCLUDES = -I $(DEPS)/libwebsock/src
LIB_WEBSOCK_LIB = -L $(DEPS)/libwebsock/src/.libs/ -lwebsock

LIB_WEBSOCK_LIB_PATH = $(DEPS)/libwebsock/src/.libs/

LIB_WEBSOCK_OBJS = \
	libwebsock_la-api.o \
	libwebsock_la-base64.o \
	libwebsock_la-default_callbacks.o \
	libwebsock_la-frames.o \
	libwebsock_la-sha1.o \
	libwebsock_la-utf.o \
	libwebsock_la-util.o \
	libwebsock_la-websock.o

INCLUDES = $(LIB_EVHTP_INCLUDES) $(LIB_WEBSOCK_INCLUDES)

all: echo-server

echo-server: echo-server.c
	# libtool --mode=link gcc -o echo echo.c $(CFLAGS) $(INCLUDES) $(LIB_WEBSOCK_LIB) ../deps/libwebsock/src/.libs/libwebsock.la
	gcc $< -o $@ $(CFLAGS) $(INCLUDES) $(patsubst %.o,$(LIB_WEBSOCK_LIB_PATH)%.o, $(LIB_WEBSOCK_OBJS)) -levent -levent_pthreads -lpthread

