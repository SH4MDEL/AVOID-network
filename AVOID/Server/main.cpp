#include "stdafx.h"
#include "Protocol.h"
#include "PlayerData.h"

SOCKET listen_sock = NULL;

CRITICAL_SECTION CS;

ServerSharedData SharedData;

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
		else
		{
			char packetType = TranslatePacket(packetDataBuf);

			GetDataFromPacket(client_sock, pBuf, packetType);
			EnterCriticalSection(&CS);

			LeaveCriticalSection(&CS);
		}
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
		dataBuf = nullptr;
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
		std::cout << "GetDataFromPacket : The Packet Type " << packetType << " is Not Exist." << std::endl;
		break;
	}
}

void ApplyPacketData(SOCKET socket, char* dataBuf, char packetType)
{
	switch (packetType)
	{
	case CS_PACKET_LOGIN: // dataBuf에는 플레이어가 선택한 음악 종류가 들어있음. 
		SharedData.PlayerJoin(socket, dataBuf);
		break;
	case CS_PACKET_READY: // dataBuf에는 데이터가 없음. 그러나, READY가 된 것이므로 플레이어의 상태를 변경해 주어야 한다.
		// 그러므로 모든 플레이어의 상태를 게임을 시작하는 상태로 변경할 필요가 있다.
		for (auto i = SharedData.m_pPlayers.begin(); i != SharedData.m_pPlayers.end(); ++i) {
			(*i).playerState = PLAYER_STATE::PLAY_GAME;
		}
		// 이후 게임을 시작된다는 패킷을 바로 보내주어야 한다.
		SharedData.CreateNewGame();
		break;
	case CS_PACKET_PLAYER_STATUS: // dataBuf에는 플레이어 넘버, 플레이어의 위치, 플레이어의 스킬 사용 여부가 들어있다.
		// 그러므로 데이터를 갱신해 주어야 한다. 임계영역이 설정되어 있으므로 공유 데이터에 문제는 없음.
		SharedData.UpdatePlayerStatus(socket, dataBuf);
		// 이후 이벤트를 활용해 Collision 쓰레드를 실행시켜야 하므로 일단 대기.
		break;
	case CS_PACKET_PLAYER_HP: // dataBuf에는 플레이어의 HP가 들어와 있다.
		// 이는 게임이 끝나고, 순위를 결정해 주기를 원하는 것.
		SharedData.GetPlayerRank(socket, dataBuf);
		break;
	case CS_PACKET_LOGOUT: // dataBuf에는 플레이어의 ID가 들어있음.
		SharedData.PlayerLeft(dataBuf);
		break;
	default:
		std::cout << "ApplyPacketData : The Packet Type " << packetType << " is Not Exist." << std::endl;
		break;
	}
}

void MakePacket() {

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
		if (SharedData.m_pPlayers.size() < MAX_USER) {
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