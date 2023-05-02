
SRCS=socket_server.cpp

all:
	g++ -g -DTHROW_EXCEPTION_ON_ERROR -Iinclude src/socket_server.cpp  test/testSocketServer.cpp -lpthread -o bin/SimpleTestSocketServer
	g++ -g -DTHROW_EXCEPTION_ON_ERROR -Iinclude src/http_server.cpp src/socket_server.cpp  test/testHttpServer.cpp -lpthread -o bin/SimpleTestHttpServer
