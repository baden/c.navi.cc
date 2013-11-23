

deps: deps/libevhtp/libevhtp.a

deps/libevhtp/libevhtp.a:
	cd deps; git clone https://github.com/ellzey/libevhtp.git; cd libevhtp/build; cmake ..; make
