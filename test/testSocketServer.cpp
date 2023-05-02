#include "socket_server.h"
#include <iostream>
#include <unistd.h>
using namespace std;

SocketServer server;

void datahandler(int clientId, void* cbData, const char* data, uint16_t dataSize)
{
	cout << "Data encountered: " << data << endl;
	string retData = "HTTP/1.1 200 OK\nContent-Type: text/raw\nContent-Length: 5\n\natula";
	server.SendResponse(clientId, retData.c_str(), retData.size());
}

int main()
{
	server.Init("9000", SS_PROTOCOLTCP | SS_REUSEADDR, &datahandler, NULL);

	while(1) { usleep(1000000); }

	server.Uninit();
}
