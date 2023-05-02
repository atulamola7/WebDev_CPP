#include "http_server.h"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <csignal>
using namespace std;

int run = 1;
void sighandler(int signal)
{
	run = false;
}

HttpServer server;

void http_handler (int clientId, void* cbData, const char* uri, const char* method, const char** headers, uint8_t numHeaders, const char* data, uint16_t dataSize)
{
	string respdata;
	const char* respCode = "404";
	const char* respStr = 0; "URI not available";
	const char** heads = NULL;
	int nhead = 0;

	char* filename = "htm_pages/notfound.html";
	if(!strcmp(method, "GET") && !strcmp(uri, "/introduction"))
	{
		filename = "htm_pages/introduction.html";
		respCode = "200";
		respStr = "OK";
		heads = headers;
		nhead = numHeaders;
	}

	ifstream file(filename, ios::ate);
	respdata.resize(file.tellg());
	file.seekg(0);
	file.read(&respdata[0], respdata.size());

	server.CreateAndSendResponse(clientId, respCode, respStr, heads, nhead, respdata.c_str(), respdata.size());
}

int main()
{
	server.Init ( "9000", &http_handler, NULL );

//	signal(SIGINT, sighandler);

	while(run) usleep(1000000);

	server.Uninit();

	cout << "Exited safely" << endl;
}
