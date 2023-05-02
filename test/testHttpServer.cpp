#include "http_server.h"
#include <iostream>
#include <unistd.h>
using namespace std;


HttpServer server;

void http_handler (int clientId, void* cbData, const char* uri, const char* method, const char** headers, uint8_t numHeaders,const char* data, uint16_t dataSize)
{

	cout << "Method: " << method << endl;
	cout << "URI   : " << uri << endl;
	cout << "Data  : " << data << endl;

	if(!strcmp(method, "GET"))
		server.CreateAndSendResponse(clientId, "405", "GET method not found", headers, numHeaders, "<HTML><BODY><p>GET method failed</p></BODY></HTML>", 50);
	else
		server.CreateAndSendResponse(clientId, "200", "OK", headers, numHeaders, "atula", 5);
}

int main()
{
	server.Init ( "9000", &http_handler, NULL );

	while(1) usleep(1000000);
}
