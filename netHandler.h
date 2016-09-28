#pragma once

#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib") 

#define FAMILY AF_INET

typedef struct sockaddr_in sockaddr_in;

class NetConn
{
	sockaddr_in*	addr;
	WSADATA			WsaData;
	SOCKET			outsock;
	char			destIP[16];
	int				result;
public:
	NetConn(const char*, const unsigned int);
	NetConn(const char*, const unsigned int, sockaddr_in*);
	int mysend(const int, float, float, float);
	int getResult() const;
};
