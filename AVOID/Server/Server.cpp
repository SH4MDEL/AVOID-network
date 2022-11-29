#include "Server.h"

/*
-------------------------------------------------------------------------------

인 게임 씬의 기본적인 흐름은

1. 세개의 클라이언트에게서 데이터를 받는다 -> 임계 영역
2. 충돌 체크 스레드로 이동하여 충돌 체크를 한다 -> 이벤트 
3. 업데이트를 통해 위치 업데이트를 해준다. 
4. 위치 데이터를 보내준다.

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
	char packetDataBuf[2] = {NULL};
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
			
			std::cout << "New Packet Received." << std::endl;
			std::cout << "New Packet Size is " << retval << "byte" << std::endl;

			char packetType = TranslatePacket(packetDataBuf);

			pBuf = GetDataFromPacket(client_sock, pBuf, packetType);
			
			EnterCriticalSection(&CS);
			ApplyPacketData(client_sock, pBuf, packetType);
			LeaveCriticalSection(&CS);

			WaitForSingleObject(hClientEvent, INFINITE);
			
			MakePacket(client_sock);
			delete[] pBuf;
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
		std::cout << "The Packet Type is CS_PACKET_LOGIN" << std::endl;
		return CS_PACKET_LOGIN;
		break;
	case CS_PACKET_READY:
		std::cout << "The Packet Type is CS_PACKET_READY" << std::endl;
		return CS_PACKET_READY;
		break;
	case CS_PACKET_PLAYER_STATUS:
		std::cout << "The Packet Type is CS_PACKET_PLAYER_STATUS" << std::endl;
		return CS_PACKET_PLAYER_STATUS;
		break;
	case CS_PACKET_PLAYER_HP:
		std::cout << "The Packet Type is CS_PACKET_PLAYER_HP" << std::endl;
		return CS_PACKET_PLAYER_HP;
		break;
	case CS_PACKET_LOGOUT:
		std::cout << "The Packet Type is CS_PACKET_LOGOUT" << std::endl;
		return CS_PACKET_LOGOUT;
		break;
	default:
		std::cout << "The Packet Type is Not Exist." << std::endl;
		return -1;
		break;
	}
}

char* GetDataFromPacket(SOCKET socket, char* dataBuf, char packetType)
{
	int retval = {0};
	switch (packetType)
	{
	case CS_PACKET_LOGIN:
		dataBuf = new char[CS_PACKET_LOGIN_SIZE];
		retval = recv(socket, dataBuf, CS_PACKET_LOGIN_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket. The Packet Type is CS_PACKET_LOGIN" << std::endl;
		}
		std::cout << "Packet Data Received." << std::endl;
		break;
	case CS_PACKET_READY:
		dataBuf = new char[CS_PACKET_READY_SIZE];
		retval = recv(socket, dataBuf, CS_PACKET_READY_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket. The Packet Type is CS_PACKET_LOGIN" << std::endl;
		}
		std::cout << "Packet Data Received." << std::endl;
		break;
	case CS_PACKET_PLAYER_STATUS:
		dataBuf = new char[CS_PACKET_PLAYER_STATUS_SIZE];
		recv(socket, dataBuf, CS_PACKET_PLAYER_STATUS_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket. The Packet Type is CS_PACKET_PLAYER_STATUS" << std::endl;
		}
		std::cout << "Packet Data Received." << std::endl;
		break;
	case CS_PACKET_PLAYER_HP:
		dataBuf = new char[CS_PACKET_PLAYER_HP_SIZE];
		recv(socket, dataBuf, CS_PACKET_PLAYER_HP_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket. The Packet Type is CS_PACKET_PLAYER_HP" << std::endl;
		}
		std::cout << "Packet Data Received." << std::endl;
		break;
	case CS_PACKET_LOGOUT:
		dataBuf = new char[CS_PACKET_LOGOUT_SIZE];
		recv(socket, dataBuf, CS_PACKET_LOGOUT_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket. The Packet Type is CS_PACKET_LOGOUT" << std::endl;
		}
		std::cout << "Packet Data Received." << std::endl;
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
	case CS_PACKET_LOGIN: // dataBuf에는 플레이어가 선택한 음악 종류가 들어있음. 
		SharedData.PlayerJoin(socket, dataBuf);
		std::cout << "Apply CS_PACKET_LOGIN Packet." << std::endl;
		std::cout << "Now the Player Count is " << SharedData.m_pPlayers.size() << std::endl;
		break;
	case CS_PACKET_READY: // 이제 dataBuf에는 playerId가 들어있다. 그러므로, 해당 데이터를 바탕으로 받아줘야 한다.
	{

		SharedData.CreateNewGame(dataBuf);
		std::cout << "Apply CS_PACKET_READY Packet." << std::endl;
	}
		break;
	case CS_PACKET_PLAYER_STATUS: // dataBuf에는 플레이어 넘버, 플레이어의 위치, 플레이어의 스킬 사용 여부가 들어있다.
		// 그러므로 데이터를 갱신해 주어야 한다. 임계영역이 설정되어 있으므로 공유 데이터에 문제는 없음.
		SharedData.UpdatePlayerStatus(socket, dataBuf);
		std::cout << "Apply CS_PACKET_PLAYER_STATUS Packet." << std::endl;
		break;
	case CS_PACKET_PLAYER_HP: // dataBuf에는 플레이어의 HP가 들어와 있다.
		// 이는 게임이 끝나고, 순위를 결정해 주기를 원하는 것.
		SharedData.GetPlayerRank(socket, dataBuf);
		std::cout << "Apply CS_PACKET_PLAYER_HP Packet." << std::endl;
		break;
	case CS_PACKET_LOGOUT: // dataBuf에는 플레이어의 ID가 들어있음.
		SharedData.PlayerLeft(dataBuf);
		std::cout << "Apply CS_PACKET_LOGOUT Packet." << std::endl;
		break;
	default:
		std::cout << "ApplyPacketData : The Packet Type " << packetType << " is Not Exist." << std::endl;
		break;
	}
}

void MakePacket(SOCKET sock) {

	// unique_ptr의 사용법을 좀더 공부해야 할듯.
	// data 변수를 unique_ptr로서 사용하고 싶었으나, 동적 할당의 경우 어떤 식으로 해야할 지 알 수 없었음.
	// 아마 make_unique 같은 함수를 바탕으로 만들어야 하는 것 같음. 
	// 그래서 일단은 평범한 동적 할당을 바탕으로 만듦.
	// 공부 열심히 할게요...

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
		std::cout << "Send SC_PACKET_LOGIN_CONFIRM." << std::endl;
		delete[] data;
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
		for (auto& i : SharedData.m_pPlayers) {
			send(i.playerSocket, data, sizeof(sc_packet_start_game), 0);
		}
		std::cout << "Send SC_PACKET_START_GAME." << std::endl;
		delete[] data;
	}
		break;
	case SC_PACKET_OBJECTS_INFO:
	{
		sc_packet_objects_info packet;
		packet.size = sizeof(sc_packet_objects_info);
		packet.type = SC_PACKET_OBJECTS_INFO;
		packet.playerNum = SharedData.m_pPlayers.size();
		packet.enemyNum = SharedData.m_pEnemies.size();
		packet.bulletNum = SharedData.GetBulletNum();
		char* data = new char[sizeof(sc_packet_objects_info)];
		memcpy(data, &packet, sizeof(sc_packet_objects_info));
		send(sock, data, sizeof(sc_packet_objects_info), 0);
		std::cout << "Send SC_PACKET_OBJECTS_INFO." << std::endl;

		PlayerStatus* playerDataBuf = new PlayerStatus[packet.playerNum];

		int i = 0;
		for (auto& player : SharedData.m_pPlayers)
		{
			playerDataBuf[i].coord = player.position;
			playerDataBuf[i].isCollide = player.isCollide;
			player.isCollide = FALSE;
			playerDataBuf[i].playerID = player.playerId;
			++i;
		}

		char* cPlayerData = reinterpret_cast<char*>(playerDataBuf);
		send(sock, cPlayerData, sizeof(PlayerStatus) * packet.playerNum, 0);
		delete[] playerDataBuf;

		Coord* enemyDataBuf = new Coord[packet.enemyNum];

		i = 0;
		for (auto& enemy : SharedData.m_pEnemies) {	
			enemyDataBuf[i] = enemy.GetPosition();
			std::cout << enemy.GetPosition().x << ", " << enemy.GetPosition().y << std::endl;
			++i;
		}

		char* cEnemyData = reinterpret_cast<char*>(enemyDataBuf);
		send(sock, cEnemyData, sizeof(Coord) * packet.enemyNum, 0);
		delete[] enemyDataBuf;

		Coord* bulletDataBuf = new Coord[packet.bulletNum];

		i = 0;
		for (auto& enemy : SharedData.m_pEnemies) {
			for (auto& bullet : enemy.GetBullets()) {
				bulletDataBuf[i] = bullet.GetPosition();
				++i;
			}
		}

		char* cBulletData = reinterpret_cast<char*>(bulletDataBuf);
		send(sock, cBulletData, sizeof(Coord) * packet.bulletNum, 0);
		delete[] bulletDataBuf;

		delete[] data;
	}
		break;
	case SC_PACKET_MUSIC_END:
	{
		sc_packet_music_end packet;
		packet.size = sizeof(sc_packet_music_end);
		packet.type = SC_PACKET_MUSIC_END;
		char* data = new char[sizeof(sc_packet_music_end)];
		memcpy(data, &packet, sizeof(sc_packet_music_end));
		send(sock, data, sizeof(sc_packet_music_end), 0);
		delete[] data;
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
		delete[] data;
	}
		break;
	case NULL:
	{
		//다음 send를 호출하기 위해 넘기는 경우 사용. (플레이어 3명에게 모두 받아야 하는 데이터의 경우)
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
		
		timeElapsed = std::chrono::system_clock::now() - currentTime;
		if (timeElapsed.count() > 1.0f / 60.0f)
		{
			currentTime = std::chrono::system_clock::now();
			SharedData.Update(timeElapsed.count());

		}
		ResetEvent(hCollideEvent);
		SetEvent(hClientEvent);
	}
}

void CollisionCheckBulletAndWall() 
{	

	// 데이터는 모두 SharedData안에 있으므로, 굳이 따로 전역변수를 다시 지정해 줄 이유는 없다.

	for (auto& enemy : SharedData.m_pEnemies) {
		for (auto bullet = enemy.GetBullets().begin(); bullet != enemy.GetBullets().end(); ++bullet) {
			Coord pos = (*bullet).GetPosition();
			int d = pos.x * pos.x + pos.y * pos.y;
			if (d > RADIUS_OF_ENEMY * RADIUS_OF_ENEMY) // 원래는 원 밖에 나가도 1초가 지나야 사라질 수 있게 했지만 그런게 필요할까?
			{
				enemy.GetBullets().erase(bullet);
				break;
			}
		}
	}

}

void CollisionCheckPlayerAndBullet() {

	double d = 0;
	double r1 = 0;
	double r2 = 0;

	for (auto& enemy : SharedData.m_pEnemies) {
		for (auto& bullet = enemy.GetBullets().begin(); bullet != enemy.GetBullets().end(); ++bullet) {
			for (auto& player : SharedData.m_pPlayers) {
				Coord bPos = (*bullet).GetPosition();
				d = sqrt((double)(pow((double)bPos.x - (double)(player.position.x), 2) + pow(bPos.y - (double)(player.position.y), 2)));
				r1 = BULLET_RADIUS + PLAYER_RADIUS;
				r2 = BULLET_RADIUS - PLAYER_RADIUS;

				if (((r2 < d && d <= r1) || r2 >= d) && !(player.isInvincible)) {
					player.isCollide = true;
					player.isInvincible = true;
					player.AttackedTime = 0.0f;
				}
			}
		}
	}
}

void CollisionCheckAbility() {

	double d = 0;
	double r1 = 0;
	double r2 = 0;

	for (auto enemy : SharedData.m_pEnemies) {
		for (auto& bullet : enemy.GetBullets()) {
			for (auto& player : SharedData.m_pPlayers) {
				Coord bPos = bullet.GetPosition();
				d = sqrt((double)(pow((double)bPos.x - (double)(player.position.x), 2) + pow(bPos.y - (double)(player.position.y), 2)));
				r1 = BULLET_RADIUS + ABILITY_RADIUS;
				r2 = BULLET_RADIUS - ABILITY_RADIUS;

				if (player.isSkill) {
					if ((r2 < d && d <= r1) || r2 >= d) {
						bullet.SetBulletSpeed(3);
					}
				}
				else {
					bullet.SetBulletSpeed(10);
				}
			}
		}
	}
}


int main(int argc, char* argv[]) {
	
	initServer();

	InitializeCriticalSection(&CS);

	hClientEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

	hCollideEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

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