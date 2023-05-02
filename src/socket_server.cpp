#include "socket_server.h"
#include "utils.h"

#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

SocketServer :: SocketServer ( void )
{
	m_serverFD = -1;
	m_handlerCB = NULL;
	m_runThreads = 1;
	m_clientThreads.resize(100, 0);
	m_clientThreadsFinished.resize(100, 0);
	m_connectedClientFDs.resize(100, -1);
}

SocketServer :: ~SocketServer ( void )
{
	Uninit();
}

int SocketServer :: Init ( const char* port, uint32_t flags, sock_data_handler handler, void *cbData )
{
	uint32_t socketFlags = 0;

	if(flags & SS_REUSEADDR)
		socketFlags |= SO_REUSEADDR;
	if(flags & SS_REUSEPORT)
		socketFlags |= SO_REUSEPORT;

	int type = 0;
	if(SS_PROTOCOLTCP & flags)
		type = SOCK_STREAM;
	else if(SS_PROTOCOLUDP & flags)
		type = SOCK_DGRAM;

	m_serverFD = socket(AF_INET, type, 0);
	HANDLE_ERROR(m_serverFD < 0, "Socket() failed", -1);

	int opt = 1;
	int ret = setsockopt(m_serverFD, SOL_SOCKET, socketFlags, &opt, sizeof(opt));
	HANDLE_ERROR(ret, "setsockopt() failed", -1);

	struct sockaddr_in serverAddress;
	serverAddress.sin_family      = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port        = htons(atoi(port));

	ret = bind(m_serverFD, (sockaddr*)&serverAddress, sizeof(serverAddress));
	HANDLE_ERROR(ret < 0, "bind() failed", -1);

	m_handlerCB = handler;
	HANDLE_ERROR(m_handlerCB == NULL, "MsgData call back not registered - failed", -1);
	m_cbData = cbData;

	m_listenerThread = std:: thread(&SocketServer::listenerThreadFunc, this);

	std:: cout << "Socket server initialized " << std:: endl;

	return 0;
}

void SocketServer :: Uninit ( void )
{
	m_runThreads = 0;
	if(m_listenerThread.joinable())
		m_listenerThread.join();

	for(int i = 0; i < m_clientThreadsFinished.size(); i++)
	{
		if(m_clientThreadsFinished[i])
		{
			if(m_clientThreadsFinished[i]->joinable())
				m_clientThreadsFinished[i]->join();

			delete m_clientThreadsFinished[i];
			m_clientThreadsFinished[i] = NULL;
		}
	}

	for(int i = 0; i < m_clientThreads.size(); i++)
	{
		if(m_clientThreads[i])
		{
			if(m_clientThreads[i]->joinable())
				m_clientThreads[i]->join();

			delete m_clientThreads[i];
			m_clientThreads[i] = NULL;
		}
	}

	shutdown(m_serverFD, SHUT_RDWR);
	m_handlerCB = NULL;
}

void SocketServer :: listenerThreadFunc ( void )
{
	while(m_runThreads)
	{
		if(listen(m_serverFD, 100) == 0)
		{
			std:: cout << "New client connection request received!" << std:: endl;
			for(int i = 0; i < m_clientThreads.size(); i++)
			{
				if(!m_clientThreads[i])
				{
					struct sockaddr_in clientAddress;
					int addrlen = sizeof(clientAddress);
					int clientFD = accept(m_serverFD, (sockaddr*)&clientAddress, (socklen_t*)&addrlen);
					m_clientThreads[i] = new std:: thread(&SocketServer::clientThreadFunc, this, clientFD, i, clientAddress);
					break;
				}
			}
		}
		else
		{
			for(int i = 0; i < m_clientThreadsFinished.size(); i++)
			{
				if(m_clientThreadsFinished[i])
				{
					if(m_clientThreadsFinished[i]->joinable())
						m_clientThreadsFinished[i]->join();

					delete m_clientThreadsFinished[i];
					m_clientThreadsFinished[i] = NULL;
				}
			}
		}
	}
}

void SocketServer :: clientThreadFunc( int clientFD, int threadId, sockaddr_in clientAddress )
{
	if(clientFD < 0)
	{
		std:: cout << "Client connection failed! Not able to accept request!" << std:: endl;
	}
	else
	{
		std:: cout << "New connection accepted at thread id: " << threadId << std:: endl;
		std:: cout << "Client Info - " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std:: endl;

		m_connectedClientFDs[threadId] = clientFD;

		int valread = 0;
		char buffer[1024] = {0};
		int maxRetries = 5;

		while(m_runThreads && maxRetries > 0)
		{
			valread = read(clientFD, buffer, 1024);

			if(valread <= 0)
				maxRetries--;
			else
				m_handlerCB(threadId, m_cbData, buffer, valread);
		}

		close(clientFD);
		std:: cout << "Connection Closed for threadid : " << threadId << " Reason: Connection Finished or Errorful connection" << std:: endl;
	}
	
	m_clientThreadsFinished[threadId] = m_clientThreads[threadId];
	m_clientThreads[threadId] = NULL;
}

int SocketServer :: Set ( const char* properties, ... )
{
	// This function should not be called
	return 0;
}

int SocketServer :: SendResponse(int clientId, const char* data, uint16_t dataSize)
{
	if( send(m_connectedClientFDs[clientId], data, dataSize, 0) == -1 )
		std:: cout << "SendResponse Failed: " << std:: endl;

	return 0;
}
