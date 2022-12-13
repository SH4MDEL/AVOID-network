#include "main.h"

Timer		g_timer;
HANDLE		g_event;

SharedData sharedData;
static bool once = false;
CRITICAL_SECTION		g_critical_section;

int main()
{

	InitializeCriticalSection(&g_critical_section);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "Init Fail." << std::endl;
		return 0;
	}

	g_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_listenSock == INVALID_SOCKET)
	{
		std::cout << "Create Socket Fail." << std::endl;
		return 0;
	}
	struct sockaddr_in serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);

	int retval = bind(g_listenSock, (sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		std::cout << "Socket Error in bind" << std::endl;
		return 0;
	}

	retval = listen(g_listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		std::cout << "Socket Error in listen" << std::endl;
		return 0;
	}

	SOCKET clientSock;
	sockaddr_in clientAddr;
	int addrlen;
	HANDLE hThread;

	hThread = CreateThread(NULL, 0, MainThread, nullptr, 0, NULL);
	g_event = CreateEvent(NULL,	TRUE, FALSE, NULL);

	while (1) {

		if (sharedData.players.size() < MAX_USER && !sharedData.musicStart) {

			addrlen = sizeof(clientAddr);

			clientSock = accept(g_listenSock, (sockaddr*)&clientAddr, &addrlen);
			if (clientSock == INVALID_SOCKET) {
				std::cout << "Accept Fail." << std::endl;
				continue;
			}

			char addr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &clientAddr.sin_addr, addr, sizeof(addr));
			std::cout << "Client Connected. IP : " << addr << ", PORT : "
				<< ntohs(clientAddr.sin_port) << std::endl;

			hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)clientSock, 0, NULL);
			if (hThread == NULL) {
				closesocket(clientSock);
			}
			else {
				CloseHandle(hThread);
			}

		}
	}
}

DWORD WINAPI MainThread(LPVOID arg)
{

	while (1) {
		if (sharedData.musicStart && !g_insertPlayerStatus.empty() &&
			g_insertPlayerStatus.size() == sharedData.players.size()) {
			WaitForSingleObject(g_event, INFINITE);
			if (!once) {
				once = true;
				g_timer.Tick();
			}
			g_timer.Tick();
			while (!g_insertPlayerStatus.empty()) {
				PlayerStatus ps = g_insertPlayerStatus.front();
				g_insertPlayerStatus.pop();

				PlayerData& pd = sharedData.GetPlayerData(ps.playerID);
				pd.position = ps.coord;
				pd.isSkill = ps.isSkill;
			}

			CollisionCheckBulletAndWall();
			CollisionCheckPlayerAndBullet();
			CollisionCheckAbility();
			sharedData.Update(g_timer.GetDeltaTime());
			ResetEvent(g_event);
		}
		if (sharedData.musicEnd &&
			g_hpData.size() == sharedData.players.size()) {
			if (once) once = false;
			std::sort(sharedData.players.begin(), sharedData.players.end(), [](const PlayerData& a, const PlayerData& b) {
				return a.Hp > b.Hp;
				});
			int rank = 0;
			sc_packet_rank packet;
			packet.type = SC_PACKET_RANK;
			packet.size = sizeof(sc_packet_rank);
			for (auto& player : sharedData.players) {
				packet.rank = ++rank;
				Send(player.socket, &packet);
			}
			sharedData.musicEnd = false;
			sharedData.ServerReset();
		}
		if (!sharedData.musicStart && !sharedData.musicEnd) {
			Sleep(1);
		}
	}
}

DWORD WINAPI ClientThread(LPVOID arg)
{
	std::cout << "Client Thread Start." << std::endl;
	SOCKET clientSock = (SOCKET)arg;

	sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;

	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while (Recv(clientSock)) {}

	return 0;
}

int Recv(SOCKET socket)
{
	packet pk;
	int retval = recv(socket, reinterpret_cast<char*>(&pk), sizeof(packet), MSG_WAITALL);
	if (retval == SOCKET_ERROR) {
		std::cout << "Socket Error in recv" << std::endl;
		WSAGetLastError();
		sharedData.PlayerLeft(socket);
		if (sharedData.players.size() <= 0) {
			sharedData.ServerReset();
			once = false;
			ResetEvent(g_event);
		}
		else {
			EnterCriticalSection(&g_critical_section);
			//g_mutex.lock();
			if (g_insertPlayerStatus.size() == sharedData.players.size()) {
				SetEvent(g_event);
			}
			//g_mutex.unlock();
			LeaveCriticalSection(&g_critical_section);
		}
		return 0;
	}
	else if (retval == 0) {
		std::cout << "No Data." << std::endl;
		return 0;
	}
	else {
#ifdef SERVER_DEBUG
		std::cout << "New Packet Received." << std::endl;
		std::cout << "New Packet Size is " << retval << "byte" << std::endl;
#endif //SERVER_DEBUG

		TranslatePacket(socket, pk);
		return 1;
	}

}

void TranslatePacket(SOCKET socket, const packet& packetBuf)
{
	int retval, remain;
	switch (packetBuf.type)
	{
	case CS_PACKET_LOGIN:
	{
		cs_packet_login pk;
		retval = recv(socket, reinterpret_cast<char*>(&pk) + 2, packetBuf.size - 2, MSG_WAITALL);
		sharedData.PlayerJoin(socket, (int)pk.selectMusic);
#ifdef SERVER_DEBUG
		cout << "CS_PACKET_LOGIN 해석" << endl;
#endif // SERVER_DEBUG
		break;
	}
	case CS_PACKET_READY:
	{
		cs_packet_ready pk;
		retval = recv(socket, reinterpret_cast<char*>(&pk) + 2, packetBuf.size - 2, MSG_WAITALL);
		sharedData.CreateNewGame();
#ifdef SERVER_DEBUG
		cout << "CS_PACKET_READY 해석" << endl;
#endif // SERVER_DEBUG
		break;
	}
	case CS_PACKET_PLAYER_STATUS:
	{
		cs_packet_player_status pk;

		remain = packetBuf.size - 2;
		while (remain > 0) {
			retval = recv(socket, reinterpret_cast<char*>(&pk) + packetBuf.size - remain, remain, MSG_WAITALL);
			remain -= retval;
		}
		PlayerStatus ps;
		ps.playerID = pk.playerID;
		ps.coord = pk.coord;
		ps.isSkill = pk.isSkill;

		EnterCriticalSection(&g_critical_section);
		//g_mutex.lock();
		g_insertPlayerStatus.push(ps);
		if (g_insertPlayerStatus.size() == sharedData.players.size()) {
			SetEvent(g_event);
		}
		//g_mutex.unlock();
		LeaveCriticalSection(&g_critical_section);
#ifdef SERVER_DEBUG
		cout << "CS_PACKET_PLAYER_STATUS 해석" << endl;
#endif // SERVER_DEBUG
		break;
	}
	case CS_PACKET_PLAYER_HP:
	{
		cs_packet_player_hp pk;
		retval = recv(socket, reinterpret_cast<char*>(&pk) + 2, packetBuf.size - 2, MSG_WAITALL);
		for (auto& player : sharedData.players) {
			if (player.ID == pk.playerID) {
				player.Hp = pk.hp;
				g_hpData.push_back((int)pk.hp);
				break;
			}
		}
#ifdef SERVER_DEBUG
		cout << "CS_PACKET_PLAYER_HP 해석" << endl;
#endif // SERVER_DEBUG
		break;
	}
	default:
		break;
	}
}

void Send(SOCKET socket, void* packetBuf)
{
	int remain, retval;
	remain = reinterpret_cast<packet*>(packetBuf)->size;
	while (remain > 0) {
		retval = send(socket, reinterpret_cast<char*>(packetBuf) + reinterpret_cast<packet*>(packetBuf)->size - remain, remain, 0);
		remain -= retval;
	}
}

void CollisionCheckBulletAndWall()
{
	for (auto& enemy : sharedData.enemys) {
		for (auto bullet = enemy.GetBullets().begin(); bullet != enemy.GetBullets().end(); ++bullet) {
			Coord pos = (*bullet).GetPosition();
			int d = pos.x * pos.x + pos.y * pos.y;
			if (d > (ENEMY_RADIUS + 20) * (ENEMY_RADIUS + 20)) // 원래는 원 밖에 나가도 1초가 지나야 사라질 수 있게 했지만 그런게 필요할까?
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

	for (auto& player : sharedData.players) {
		if (player.isCollide) player.isCollide = false;
	}
	for (auto& enemy : sharedData.enemys) {
		for (auto& bullet = enemy.GetBullets().begin(); bullet != enemy.GetBullets().end(); ++bullet) {
			for (auto& player : sharedData.players) {
				Coord bPos = (*bullet).GetPosition();
				d = sqrt((double)(pow((double)bPos.x - (double)(player.position.x), 2) + pow(bPos.y - (double)(player.position.y), 2)));
				r1 = BULLET_RADIUS + PLAYER_RADIUS;
				r2 = BULLET_RADIUS - PLAYER_RADIUS;

				if (((r2 < d && d <= r1) || r2 >= d) && !(player.isInvincible)) {
					player.isCollide = true;
					player.isInvincible = true;
					player.invincibleTime = 0.f;
				}
			}
		}
	}
}

void CollisionCheckAbility() {

	double d = 0;
	double r1 = 0;
	double r2 = 0;

	for (auto& enemy : sharedData.enemys) {
		for (auto& bullet : enemy.GetBullets()) {
			bullet.SetSpeed(15);
		}
	}
	for (auto& enemy : sharedData.enemys) {
		for (auto& bullet : enemy.GetBullets()) {
			for (auto& player : sharedData.players) {
				if (player.isSkill) {
					Coord bPos = bullet.GetPosition();
					d = sqrt((double)(pow((double)bPos.x - (double)(player.position.x), 2) + pow(bPos.y - (double)(player.position.y), 2)));
					r1 = BULLET_RADIUS + ABILITY_RADIUS;
					r2 = BULLET_RADIUS - ABILITY_RADIUS;

					if ((r2 < d && d <= r1) || r2 >= d) {
						bullet.SetSpeed(5);
					}
				}
			}
		}
	}
}
