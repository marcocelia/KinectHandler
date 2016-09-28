#include "netHandler.h"

NetConn::NetConn(const char* destIP, const unsigned int destport)
{
	if (WSAStartup(0x202, &WsaData) == SOCKET_ERROR)
	{
		std::cout << "[PacketClient] WSAStartup failed (error: " << WSAGetLastError() << ")" << std::endl;
		result = -1;
		WSACleanup();
	}
	else
	{
		outsock = socket(AF_INET, SOCK_DGRAM, 0);
		sprintf(this->destIP, destIP);
		sockaddr_in a;
		memset(&a, 0, sizeof(addr));		
		a.sin_family = AF_INET;
		a.sin_port = htons(destport);
		a.sin_addr.s_addr = inet_addr(this->destIP);
		result = connect(outsock, (struct sockaddr*)&a, sizeof(a));
		addr = &a;
	}
}
NetConn::NetConn(const char* destIP, const unsigned int destport, sockaddr_in* a)
{
	if (WSAStartup(0x202, &WsaData) == SOCKET_ERROR)
	{
		std::cout << "[PacketClient] WSAStartup failed (error: " << WSAGetLastError() << ")" << std::endl;
		result = -1;
		WSACleanup();
	}
	else
	{
		outsock = socket(FAMILY, SOCK_DGRAM, 0);
		addr = a;
		result = connect(outsock, (struct sockaddr*)addr, sizeof(addr));
	}
}

int NetConn::mysend(const int ID, float xpos, float ypos, float zpos)
{
	char payload[512];
	sprintf(payload, "VR_TRACKER_SENSOR_%i, %3.4f, %3.4f, %3.4f\0", ID, xpos, ypos, zpos);
	int ret = send(outsock, payload, strlen(payload), 0);
	return ret;
}

int NetConn::getResult() const
{
	return result;
}