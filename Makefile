

deps: deps/libevhtp/libevhtp.a deps/libwebsock/src/.libs/libwebsock.so

deps/libevhtp/libevhtp.a:
	cd deps; git clone https://github.com/ellzey/libevhtp.git; cd libevhtp/build; cmake ..; make

deps/libwebsock/src/.libs/libwebsock.so:
	cd deps; git clone https://github.com/payden/libwebsock.git; cd libwebsock; ./autogen.sh ; ./configure ; make
