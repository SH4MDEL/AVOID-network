#include "Server.h"

/*
-------------------------------------------------------------------------------

�� ���� ���� �⺻���� �帧��

1. ������ Ŭ���̾�Ʈ���Լ� �����͸� �޴´� -> �Ӱ� ����
2. �浹 üũ ������� �̵��Ͽ� �浹 üũ�� �Ѵ� -> �̺�Ʈ 
3. ������Ʈ�� ���� ��ġ ������Ʈ�� ���ش�. 
4. ��ġ �����͸� �����ش�.

-------------------------------------------------------------------------------
*/

SOCKET listen_sock = NULL;

CRITICAL_SECTION CS;

ServerSharedData SharedData;

DWORD WINAPI Client_Thread(LPVOID arg)
{
	std::cout << "Client Thread Start." << std::endl;
	SOCKET client_sock = (SOCKET)arg;
	int retval;

	sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char packetDataBuf[2];
	char* pBuf = nullptr;

	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	std::cout << "Network loop Start" << std::endl;

	while (true) {
		
		
		retval = recv(client_sock, packetDataBuf, 2, MSG_WAITALL);
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

			pBuf = GetDataFromPacket(client_sock, pBuf, packetType);
			
			EnterCriticalSection(&CS);
			ApplyPacketData(client_sock, pBuf, packetType);
			LeaveCriticalSection(&CS);

			WaitForSingleObject(hClientEvent, INFINITE);

			MakePacket(client_sock);
		}
	}

	return 0;
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

char* GetDataFromPacket(SOCKET socket, char* dataBuf, char packetType)
{
	switch (packetType)
	{
	case CS_PACKET_LOGIN:
		dataBuf = new char[CS_PACKET_LOGIN_SIZE];
		recv(socket, dataBuf, CS_PACKET_LOGIN_SIZE, MSG_WAITALL);
		break;
	case CS_PACKET_READY:
		dataBuf = nullptr;
		break;
	case CS_PACKET_PLAYER_STATUS:
		dataBuf = new char[CS_PACKET_PLAYER_STATUS_SIZE];
		recv(socket, dataBuf, CS_PACKET_PLAYER_STATUS_SIZE, MSG_WAITALL);
		break;
	case CS_PACKET_PLAYER_HP:
		dataBuf = new char[CS_PACKET_PLAYER_HP_SIZE];
		recv(socket, dataBuf, CS_PACKET_PLAYER_HP_SIZE, MSG_WAITALL);
		break;
	case CS_PACKET_LOGOUT:
		dataBuf = new char[CS_PACKET_LOGOUT_SIZE];
		recv(socket, dataBuf, CS_PACKET_LOGOUT_SIZE, MSG_WAITALL);
		break;
	default:
		std::cout << "GetDataFromPacket : The Packet Type " << packetType << " is Not Exist." << std::endl;
		break;
	}

	return dataBuf;
}

void ApplyPacketData(SOCKET socket, char* dataBuf, char packetType)
{
	switch (packetType)
	{
	case CS_PACKET_LOGIN: // dataBuf���� �÷��̾ ������ ���� ������ �������. 
		SharedData.PlayerJoin(socket, dataBuf);
		break;
	case CS_PACKET_READY: // dataBuf���� �����Ͱ� ����. �׷���, READY�� �� ���̹Ƿ� �÷��̾��� ���¸� ������ �־�� �Ѵ�.
		// �׷��Ƿ� ��� �÷��̾��� ���¸� ������ �����ϴ� ���·� ������ �ʿ䰡 �ִ�.
	{
		for (auto i = SharedData.m_pPlayers.begin(); i != SharedData.m_pPlayers.end(); ++i) {
			(*i).playerState = PLAYER_STATE::PLAY_GAME;
		}

		int selected = 0;

		if ((*(SharedData.m_pPlayers.begin())).selectedMusic == SELECTED_MUSIC::BBKKBKK) {
			selected = 0;
		}
		else if ((*(SharedData.m_pPlayers.begin())).selectedMusic == SELECTED_MUSIC::TRUE_BLUE) {
			selected = 1;
		}
		SharedData.CreateNewGame(selected);
	}
		break;
	case CS_PACKET_PLAYER_STATUS: // dataBuf���� �÷��̾� �ѹ�, �÷��̾��� ��ġ, �÷��̾��� ��ų ��� ���ΰ� ����ִ�.
		// �׷��Ƿ� �����͸� ������ �־�� �Ѵ�. �Ӱ迵���� �����Ǿ� �����Ƿ� ���� �����Ϳ� ������ ����.
		SharedData.UpdatePlayerStatus(socket, dataBuf);
		// ���� �̺�Ʈ�� Ȱ���� Collision �����带 ������Ѿ� �ϹǷ� �ϴ� ���.
		break;
	case CS_PACKET_PLAYER_HP: // dataBuf���� �÷��̾��� HP�� ���� �ִ�.
		// �̴� ������ ������, ������ ������ �ֱ⸦ ���ϴ� ��.
		SharedData.GetPlayerRank(socket, dataBuf);
		break;
	case CS_PACKET_LOGOUT: // dataBuf���� �÷��̾��� ID�� �������.
		SharedData.PlayerLeft(dataBuf);
		break;
	default:
		std::cout << "ApplyPacketData : The Packet Type " << packetType << " is Not Exist." << std::endl;
		break;
	}
}

void MakePacket(SOCKET sock) {

	// unique_ptr�� ������ ���� �����ؾ� �ҵ�.
	// data ������ unique_ptr�μ� ����ϰ� �;�����, ���� �Ҵ��� ��� � ������ �ؾ��� �� �� �� ������.
	// �Ƹ� make_unique ���� �Լ��� �������� ������ �ϴ� �� ����. 
	// �׷��� �ϴ��� ����� ���� �Ҵ��� �������� ����.
	// ���� ������ �ҰԿ�...

	switch (SharedData.nextPacket) {
	case SC_PACKET_LOGIN_CONFIRM:
	{
		sc_packet_login_confirm packet;
		packet.playerID = SharedData.nextPacketPlayerId;
		packet.size = sizeof(sc_packet_login_confirm);
		packet.type = SC_PACKET_LOGIN_CONFIRM;
		char* data = new char[sizeof(sc_packet_login_confirm)];
		memcpy(data, &packet, sizeof(sc_packet_login_confirm));
		send(sock, data, sizeof(sc_packet_login_confirm), 0);
		
	}
		break;
	case SC_PACKET_START_GAME:
	{
		sc_packet_start_game packet;
		packet.playerNum = SharedData.m_pPlayers.size();
		packet.size = sizeof(sc_packet_start_game);
		packet.type = SC_PACKET_START_GAME;
		char* data = new char[sizeof(sc_packet_start_game)];
		memcpy(data, &packet, sizeof(sc_packet_start_game));
		send(sock, data, sizeof(sc_packet_start_game), 0);
		delete[] data;
	}
		break;
	case SC_PACKET_OBJECTS_INFO:
	{
		sc_packet_objects_info packet;
		packet.playerNum = SharedData.m_pPlayers.size();
		packet.size = sizeof(sc_packet_objects_info);
		packet.type = SC_PACKET_START_GAME;
		char* data = new char[sizeof(sc_packet_objects_info)];
		memcpy(data, &packet, sizeof(sc_packet_objects_info));
		send(sock, data, sizeof(sc_packet_objects_info), 0);
		
		for (auto& i : SharedData.m_pPlayers)
		{
			PlayerStatus playerStatus;
			playerStatus.coord.x = i.position.x;
			playerStatus.coord.y = i.position.y;
			playerStatus.isCollide = i.isCollide;
			i.isCollide = FALSE;
			playerStatus.playerID = i.playerId;
			char* player = new char[sizeof(PlayerStatus)];
			ZeroMemory(player, sizeof(PlayerStatus));
			send(sock, player, sizeof(PlayerStatus), 0);
			delete[] player;
		}
		delete[] data;
	}
		break;
	case SC_PACKET_MUSIC_END:
	{
		sc_packet_music_end packet;
		packet.size = sizeof(sc_packet_music_end);
		packet.type = SC_PACKET_START_GAME;
		char* data = new char[sizeof(sc_packet_music_end)];
		memcpy(data, &packet, sizeof(sc_packet_music_end));
		send(sock, data, sizeof(sc_packet_music_end), 0);
	}
		break;
	case SC_PACKET_RANK:
	{
		sc_packet_rank packet;
		packet.size = sizeof(sc_packet_rank);
		packet.type = SC_PACKET_RANK;
		char* data = new char[sizeof(sc_packet_rank)];
		memcpy(data, &packet, sizeof(sc_packet_rank));
		send(sock, data, sizeof(sc_packet_rank), 0);
	}
		break;
	case SC_PACKET_LOGOUT:
	{
		// ���� ������ �ʿ䰡 �ִ°�?
		// Ŭ���̾�Ʈ ������ �α׾ƿ��� �ϴ� ��� �׳� ���� �������� �Ǵ� ���̶�� ����.
	}
		break;
	}
}

DWORD WINAPI Collision_Thread(LPVOID arg)
{
	while (true) {
		WaitForSingleObject(hCollideEvent, INFINITE);
		CollisionCheckBulletAndWall();
		CollisionCheckPlayerAndBullet();
		CollisionCheckAbility();
		SharedData.Update();
		SetEvent(hClientEvent);
	}
}

void CollisionCheckBulletAndWall() {

}

void CollisionCheckPlayerAndBullet() {

}

void CollisionCheckAbility() {

}


int main(int argc, char* argv[]) {
	
	initServer();

	InitializeCriticalSection(&CS);

	hClientEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	hCollideEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

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

			hThread = CreateThread(NULL, 0, Collision_Thread, NULL, 0, NULL);
		}
	}

	DeleteCriticalSection(&CS);

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