#include "stdafx.h"
#include "Protocol.h"

SOCKET listen_sock = NULL;

CRITICAL_SECTION cs;

DWORD WINAPI Client_Thread(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;

	sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char packetDataBuf[2];
	char* pBuf;

	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while (true) {
		retval = recv(client_sock, packetDataBuf, 2, 0);
		if (retval == SOCKET_ERROR) {
			std::cout << "Socket Error in recv" << std::endl;
			break;
		}
		else if (retval == 0) {
			std::cout << "No Data." << std::endl;
			break;
		}

		char packetType = TranslatePacket(packetDataBuf);

		GetDataFromPacket(client_sock, pBuf, packetType);
		EnterCriticalSection(&cs);

		LeaveCriticalSection(&cs);
	}


}

char TranslatePacket(char* packetBuf)
{
	int size{ 0 };
	int type{ 0 };

	memcpy(&size, &packetBuf[0], sizeof(unsigned char));
	memcpy(&type, &packetBuf[1], sizeof(char));

	switch (type)
	{
	case CS_PACKET_LOGIN:
		return CS_PACKET_LOGIN;
		break;
	case CS_PACKET_READY:
		return CS_PACKET_READY;
		break;
	case CS_PACKET_PLAYER_STATUS:
		return CS_PACKET_PLAYER_STATUS;
		break;
	case CS_PACKET_PLAYER_HP:
		return CS_PACKET_PLAYER_HP;
		break;
	case CS_PACKET_LOGOUT:
		return CS_PACKET_LOGOUT;
		break;
	default:
		return -1;
		break;
	}
}

void GetDataFromPacket(SOCKET socket, char* dataBuf, char packetType)
{
	switch (packetType)
	{
	case CS_PACKET_LOGIN:
		recv(socket, dataBuf, CS_PACKET_LOGIN_SIZE, 0);
		break;
	case CS_PACKET_READY:
		break;
	case CS_PACKET_PLAYER_STATUS:
		recv(socket, dataBuf, CS_PACKET_PLAYER_STATUS_SIZE, 0);
		break;
	case CS_PACKET_PLAYER_HP:
		recv(socket, dataBuf, CS_PACKET_PLAYER_HP_SIZE, 0);
		break;
	case CS_PACKET_LOGOUT:
		recv(socket, dataBuf, CS_PACKET_LOGOUT_SIZE, 0);
		break;
	default:
		std::cout << "GetDataFromPacket : The Packet Type Do Not Exist." << std::endl;
		break;
	}
}

DWORD WINAPI Collision_THREAD(LPVOID arg)
{

}



int main(int argc, char* argv[]) {
	
	initServer();

	SOCKET client_sock;
	sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			std::cout << "Accept Fail." << std::endl;
			continue;
		}

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		std::cout << "Client Connected. IP : " << addr << ", PORT : " 
			<< ntohs(clientaddr.sin_port) << std::endl;

		hThread = CreateThread(NULL, 0, Client_Thread, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) {
			closesocket(client_sock);
		}
		else {
			CloseHandle(hThread);
		}

	}

	closesocket(listen_sock);

	WSACleanup();
}

void initServer()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "Init Fail." << std::endl;
		return;
	}

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		std::cout << "Create Socket Fail." << std::endl;
		return;
	}

	struct sockaddr_in serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);

	int retval = bind(listen_sock, (sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		std::cout << "Socket Error in bind" << std::endl;
		return;
	}

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		std::cout << "Socket Error in listen" << std::endl;
		return;
	}

}