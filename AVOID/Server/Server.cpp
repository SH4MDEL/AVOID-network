#include "Server.h"

/*
-----------------------------------------------------------------------------------------

1. 클라이언트에게서 데이터를 받고 적용 -> 임계 영역
2. 충돌 체크 -> 이벤트 
3. 업데이트
4. 위치 데이터 송신

-----------------------------------------------------------------------------------------
*/

SOCKET listen_sock = NULL;

CRITICAL_SECTION CS;
CRITICAL_SECTION CS2;

ServerSharedData SharedData;

bool ThreadStart = false;

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

	while (true) {
			
		retval = recv(client_sock, packetDataBuf, 2, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Socket Error in recv" << std::endl;
			WSAGetLastError();
			SharedData.PlayerLeft(client_sock);
			return -1;
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
			
			EnterCriticalSection(&CS);
			MakePacket(client_sock);
			LeaveCriticalSection(&CS);

			delete[] pBuf;
		}

		
	}

	return 0;
}

char TranslatePacket(char* packetBuf)
{
	int size = packetBuf[0];
	int type = packetBuf[1];

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
	int retval = { 0 };

	switch (packetType)
	{
	case CS_PACKET_LOGIN:
	{
		dataBuf = new char[CS_PACKET_LOGIN_SIZE];
		retval = recv(socket, dataBuf, CS_PACKET_LOGIN_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket CS_PACKET_LOGIN. " << std::endl;
		}
	}
		break;
	case CS_PACKET_READY:
	{
		dataBuf = new char[CS_PACKET_READY_SIZE];
		retval = recv(socket, dataBuf, CS_PACKET_READY_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket CS_PACKET_LOGIN." << std::endl;
		}
	}
		break;
	case CS_PACKET_PLAYER_STATUS:
	{
		dataBuf = new char[CS_PACKET_PLAYER_STATUS_SIZE];
		recv(socket, dataBuf, CS_PACKET_PLAYER_STATUS_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket CS_PACKET_PLAYER_STATUS. " << std::endl;
		}
	}
		break;
	case CS_PACKET_PLAYER_HP:
	{
		dataBuf = new char[CS_PACKET_PLAYER_HP_SIZE];
		recv(socket, dataBuf, CS_PACKET_PLAYER_HP_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket CS_PACKET_PLAYER_HP. " << std::endl;
		}
	}
		break;
	case CS_PACKET_LOGOUT:
	{
		dataBuf = new char[CS_PACKET_LOGOUT_SIZE];
		recv(socket, dataBuf, CS_PACKET_LOGOUT_SIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			std::cout << "Error In GetDataFromPacket CS_PACKET_LOGOUT. " << std::endl;
		}
	}
		break;
	default:
		break;
	}

	std::cout << "Packet Data Received." << std::endl;

	return dataBuf;
}

void ApplyPacketData(SOCKET socket, char* dataBuf, char packetType)
{
	switch (packetType)
	{
	case CS_PACKET_LOGIN:
	{
		SharedData.PlayerJoin(socket, dataBuf);
	}
		break;
	case CS_PACKET_READY:
	{
		SharedData.CreateNewGame(dataBuf);
	}
		break;
	case CS_PACKET_PLAYER_STATUS:
	{
		SharedData.UpdatePlayerStatus(socket, dataBuf);
	}
		break;
	case CS_PACKET_PLAYER_HP:
	{
		SharedData.GetPlayerRank(socket, dataBuf);
	}
		break;
	default:
		std::cout << "ApplyPacketData : The Packet Type " << packetType << " is Not Exist." << std::endl;
		break;
	}
}

void MakePacket(SOCKET sock) {

	std::cout << (int)SharedData.nextPacket << std::endl;
	switch (SharedData.nextPacket) {
	case SC_PACKET_LOGIN_CONFIRM:
	{
		sc_packet_login_confirm packet;
		packet.playerID = SharedData.nextPacketPlayerId;
		packet.size = sizeof(sc_packet_login_confirm);
		packet.type = SC_PACKET_LOGIN_CONFIRM;
		send(sock, reinterpret_cast<char*>(&packet), sizeof(sc_packet_login_confirm), 0);
		std::cout << "Send SC_PACKET_LOGIN_CONFIRM." << std::endl;
	}
		break;
	case SC_PACKET_START_GAME:
	{
		sc_packet_start_game packet;
		packet.playerNum = SharedData.m_pPlayers.size();
		packet.size = sizeof(sc_packet_start_game);
		packet.type = SC_PACKET_START_GAME;
		for (auto& i : SharedData.m_pPlayers) {
			send(i.playerSocket, reinterpret_cast<char*>(&packet), sizeof(sc_packet_start_game), 0);
		}
		std::cout << "Send SC_PACKET_START_GAME." << std::endl;
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

		send(sock, reinterpret_cast<char*>(&packet), sizeof(sc_packet_objects_info), 0);
		std::cout << "Send SC_PACKET_OBJECTS_INFO." << std::endl;


		//////////////////////////////////////////////
		//Player Data


		PlayerStatus playerData[MAX_USER] = {};

		int i = 0;
		for (auto& player : SharedData.m_pPlayers)
		{
			playerData[i].coord = player.position;

			playerData[i].isCollide = player.isCollide;
			player.isCollide = FALSE;

			playerData[i].playerID = player.playerId;
#ifdef NetworkDebug
			for (auto& player : SharedData.m_pPlayers)
			{
				std::cout << player.playerId << " : " << player.position.x << ", " << player.position.y << std::endl;
			}
#endif
			++i;
		}
		
		send(sock, reinterpret_cast<char*>(playerData), sizeof(PlayerStatus) * packet.playerNum, 0);


		//////////////////////////////////////////////
		//Enemy Data

		Coord enemyDataBuf[24] = { 0, 0 };

		i = 0;
		for (auto& enemy : SharedData.m_pEnemies) {	
			enemyDataBuf[i] = enemy.GetPosition();
			++i;
		}

		send(sock, reinterpret_cast<char*>(enemyDataBuf), sizeof(Coord) * packet.enemyNum, 0);


		//////////////////////////////////////////////
		//Bullet Data

		Coord bulletDataBuf[100] = { 0, 0 };

		i = 0;
		for (auto& enemy : SharedData.m_pEnemies) {
			for (auto& bullet : enemy.GetBullets()) {
				
				bulletDataBuf[i] = bullet.GetPosition();
#ifdef NetworkDebug
				std::cout << bullet.GetPosition().x << ", " << bullet.GetPosition().y << std::endl;
#endif

				++i;
			}
		}

		send(sock, reinterpret_cast<char*>(bulletDataBuf), sizeof(Coord) * packet.bulletNum, 0);
	}
		break;
	case SC_PACKET_MUSIC_END:
	{
		sc_packet_music_end packet;
		packet.size = sizeof(sc_packet_music_end);
		packet.type = SC_PACKET_MUSIC_END;
		send(sock, reinterpret_cast<char*>(&packet), sizeof(sc_packet_music_end), 0);
	}
		break;
	case SC_PACKET_RANK:
	{
		sc_packet_rank packet;
		packet.size = sizeof(sc_packet_rank);
		packet.type = SC_PACKET_RANK;
		send(sock, reinterpret_cast<char*>(&packet), sizeof(sc_packet_rank), 0);
	}
		break;
	case NULL:
	{
	}
		break;
	}
}

DWORD WINAPI Collision_Thread(LPVOID arg)
{
	
#ifdef NetworkDebug
	std::cout << "Collision Thread 작동" << std::endl;
#endif

	if (!ThreadStart) {
		currentTime = std::chrono::system_clock::now();
		ThreadStart = true;
	}

	if (SharedData.CheckAllPlayerStatusReady())
	{
		CollisionCheckBulletAndWall();
		CollisionCheckPlayerAndBullet();
		CollisionCheckAbility();
#ifdef NetworkDebug
		std::cout << "충돌체크 작동" << std::endl;
#endif

		timeElapsed = std::chrono::system_clock::now() - currentTime;
		if (timeElapsed.count() > 1.0f / 30.0f)
		{
			currentTime = std::chrono::system_clock::now();
			SharedData.Update(timeElapsed.count());
#ifdef NetworkDebug
			std::cout << "업데이트 작동" << std::endl;
#endif

		}
	}
	ResetEvent(hCollideEvent);
	SetEvent(hClientEvent);
#ifdef NetworkDebug
	std::cout << "충돌 이벤트 비신호 및 클라이언트 이벤트 신호" << std::endl;
#endif
	return 0;
}

void CollisionCheckBulletAndWall() 
{	

	// 데이터는 모두 SharedData안에 있으므로, 굳이 따로 전역변수를 다시 지정해 줄 이유는 없다.

	for (auto& enemy : SharedData.m_pEnemies) {
		for (auto bullet = enemy.GetBullets().begin(); bullet != enemy.GetBullets().end(); ++bullet) {
			Coord pos = (*bullet).GetPosition();
			int d = pos.x * pos.x + pos.y * pos.y;
			if (d > (RADIUS_OF_ENEMY + 20) * (RADIUS_OF_ENEMY + 20)) // 원래는 원 밖에 나가도 1초가 지나야 사라질 수 있게 했지만 그런게 필요할까?
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
	InitializeCriticalSection(&CS2);

	hClientEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	hCollideEvent = CreateEvent(NULL, TRUE, FALSE, NULL);



	currentTime = std::chrono::system_clock::now();

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