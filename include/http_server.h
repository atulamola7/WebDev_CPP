#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "socket_server.h"

typedef void (*http_data_handler) (	int clientId, void *cbData, 
																		const char* uri, const char* method, 
																		const char** headers, uint8_t numHeaders,
																		const char* data, uint16_t dataSize);

class HttpServer : protected SocketServer
{
	public:
		HttpServer ( void );
		~HttpServer ( void );

		int  Init ( const char* port, http_data_handler handler, void* cbData );
		void Uninit ( void );
		int  Set ( const char* properties, ... );

		int CreateAndSendResponse ( int clientId, 
																const char* respCode, const char* respStr, 
																const char** headers, uint8_t numHeaders, 
																const char* data, uint16_t dataSize );

	protected:

		static void socketDataHandlerFn ( int, void*, const char*, uint16_t );
	
		http_data_handler m_httpHandlerCB;
		void *m_cbData;
};

#endif // _HTTP_SERVER_H_
