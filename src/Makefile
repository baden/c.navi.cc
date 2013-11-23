
CFLAGS = -O3

# Библиотеки
LIB_EVHTP_INCLUDES = -I ../deps/libevhtp -I ../deps/libevhtp/build -I ../deps/libevhtp/evthr -I ../deps/libevhtp/htparse -I ../deps/libevhtp/oniguruma
LIB_EVHTP_LIB = ../deps/libevhtp/build/libevhtp.a

INCLUDES = $(LIB_EVHTP_INCLUDES)

all: cnavicc

cnavicc: http-server.c
	gcc -o cnavicc http-server.c $(CFLAGS) $(INCLUDES) $(LIB_EVHTP_LIB) -levent -levent_pthreads -lpthread


