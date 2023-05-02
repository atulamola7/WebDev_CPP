#ifndef _SOCKET_SERVER_H_
#define _SOCKET_SERVER_H_

#include <cstdio>
#include <cstdint>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <cstring>

#include "utils.h"

typedef void (*sock_data_handler)(int clientId, void *cbData, const char* data, uint16_t dataSize);

class SocketServer
{
	public:
		SocketServer ( void );
		virtual ~SocketServer ( void );

		virtual int  Init ( const char* port, uint32_t flags, sock_data_handler handler, void* cbData );
		virtual void Uninit ( void );
		virtual int  Set ( const char* properties, ... );

		virtual int SendResponse ( int clientId, const char* data, uint16_t dataSize);

	protected:

		std:: thread m_listenerThread;
		std:: vector <std:: thread*> m_clientThreads;
		std:: vector <std:: thread*> m_clientThreadsFinished;
		std:: vector <int> m_connectedClientFDs;
		bool m_runThreads;

		void clientThreadFunc ( int, int, sockaddr_in );
		void listenerThreadFunc ( void );

		int m_serverFD;

		sock_data_handler m_handlerCB;
		void *m_cbData;
};

#endif // _SOCKET_SERVER_H_
