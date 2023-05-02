#include "http_server.h"

#include <iostream>
#include <sstream>

HttpServer :: HttpServer ( void ) : SocketServer ()
{
	m_httpHandlerCB = NULL;
	m_cbData = NULL;
}

HttpServer :: ~HttpServer ( void )
{
	HttpServer :: Uninit();
}

int HttpServer :: Init( const char* port, http_data_handler handler, void *cbData)
{
	m_httpHandlerCB = handler;
	m_cbData = cbData;
	 
	return SocketServer :: Init (port, SS_REUSEADDR | SS_PROTOCOLTCP, 
																&HttpServer::socketDataHandlerFn, this);
}

void HttpServer :: Uninit ( void )
{
	SocketServer :: Uninit();
	m_httpHandlerCB = NULL;
	m_cbData = NULL;
}

int HttpServer :: CreateAndSendResponse ( int clientId, 
																					const char* respCode, const char* respStr, 
																					const char** headers, uint8_t numHeaders,
																					const char* data, uint16_t dataSize)
{
	std:: string response;
	response.reserve(1024);
	response = "HTTP/1.1";

	if(respCode)
		response += std:: string(" ") + respCode;
	if(respStr)
		response += std:: string(" ") + respStr;
	response += "\n";

	for( int i = 0; i < numHeaders; i+=2 )
	{
		if(headers[i])
			response += headers[i] + std:: string(" ");
		if(headers[i] && headers[i+1])
			response += headers[i+1];
		if(headers[i])
			response += "\n";
	}

	char tempData[200] = {0};
	sprintf(tempData, "Content-Length: %d\n", dataSize);
	response += tempData;

	response += "\n";
	response.append(data, dataSize);

	return SocketServer::SendResponse(clientId, response.c_str(), response.size());
}

void HttpServer :: socketDataHandlerFn ( int clientId, void *cbData, 
																				 const char* data, uint16_t dataSize)
{
	std:: string recvData(data, dataSize);
	std:: istringstream isstr(recvData);


	std:: string uri, method, rdata, protocol;
	char* headers[256] = {0};
	int headersParsed = -1;
	int parsedData = 0;

	int currentParseState = 0; // 0 = parsing method + parsing uri + parsing protocol, 3 = parsing heaaders, 4 = parsing data
	while(isstr.good() && !isstr.eof())
	{
		std:: string lineData;
		getline(isstr, lineData);
		std:: istringstream iss1(lineData);

		switch(currentParseState)
		{
			case 0: iss1 >> method;
							iss1 >> uri;
							iss1 >> protocol;
							currentParseState = 3;
							break;

			case 3:	parsedData = isstr.tellg();
							if(lineData.empty() || (lineData.size() == 1 && (lineData[0] == '\n' || lineData[0] == '\r')))
							{
								currentParseState = 4;
							}
							else
							{
								headers[++headersParsed] = new char [1024];
								iss1 >> headers[headersParsed];
								headers[++headersParsed] = new char [1024];
								strncpy(headers[headersParsed], &lineData[iss1.tellg()], 1024);
							}

							break;

			case 4: 
							if(parsedData >= 0 && parsedData < recvData.size())
								rdata = recvData.substr(parsedData);
							isstr.setstate(std:: ios_base:: eofbit);
			}
	}

	char* headersList[256] = {0};
	for(int i = 0; i < headersParsed; i++)
		headersList[i] = headers[i];

	((HttpServer*)cbData)->m_httpHandlerCB(clientId, cbData, uri.c_str(), method.c_str(), (const char**)&headersList[0], 
																									headersParsed, rdata.c_str(), rdata.size());


	for(int i = 0; i < headersParsed+1; i++)
		if(headers[i])
			delete[] headers[i];
}

int HttpServer :: Set ( const char* properties, ... )
{
	return 0;
}
