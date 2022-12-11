#include "SharedData.h"

queue<PlayerStatus>		g_insertPlayerStatus;
vector<int>				g_hpData;
mutex					g_mutex;

SharedData::SharedData() : musicStart(false), musicEnd(false), tick(0), timeDelay(-1.f)
{
}

void SharedData::Update(float timeElapsed)
{
	timeDelay += timeElapsed;
	if (timeDelay >= 0.f) {
		if (g_selectedMusic == 0) {
			if (timeDelay >= (60.f / 680.f) * (float)tick) {
				int i = 0;
				for (auto& enemy : enemys) {
					enemy.SetNextNode(musicSpeed[tick], musicNote[tick][i]);
					++i;
				}
				++tick;
			}
		}
		else if (g_selectedMusic == 1) {
			if (timeDelay >= (60.f / 656.f) * (float)tick) {
				int i = 0;
				for (auto& enemy : enemys) {
					enemy.SetNextNode(musicSpeed[tick], musicNote[tick][i]);
					++i;
				}
				++tick;
			}
		}
	}

	for (auto& player : players) {
		if (player.isInvincible) {
			player.invincibleTime += timeElapsed;
			if (player.invincibleTime >= 2.0f) {
				player.isInvincible = false;
				player.invincibleTime = 0.f;
			}
		}
	}

	for (auto& enemy : enemys) {
		enemy.Update(timeElapsed);
	}


	if ((g_selectedMusic == 0 && tick < 1450) || 
		(g_selectedMusic == 1 && tick < 1310)) {

		sc_packet_objects_info packet;
		packet.type = SC_PACKET_OBJECTS_INFO;
		packet.size = sizeof(sc_packet_objects_info);
		packet.playerNum = players.size();
		packet.enemyNum = enemys.size();
		packet.bulletNum = GetBulletNum();

		for (auto& player : players) {
			Send(player.socket, &packet);
		}
#ifdef SERVER_DEBUG
		cout << "SC_PACKET_OBJECTS_INFO 전송" << endl;
#endif // SERVER_DEBUG

		PlayerStatus ps[MAX_USER];
		int i = 0;
		for (const auto& player : players)
		{
			ps[i].coord = player.position;
			ps[i].isCollide = player.isCollide;
			ps[i].playerID = player.ID;
			ps[i].isSkill = player.isSkill;
			++i;
		}

		Coord es[ENEMY_NUM];
		i = 0;
		for (const auto& enemy : enemys) {
			es[i] = enemy.GetPosition();
			++i;
		}

		Coord bs[100];
		i = 0;
		for (auto& enemy : enemys) {
			for (const auto& bullet : enemy.GetBullets()) {
				bs[i] = bullet.GetPosition();
				++i;
			}
		}

		for (auto& player : players) {
			int retval, remain;

			remain = sizeof(PlayerStatus) * packet.playerNum;
			while (remain > 0) {
				retval = send(player.socket, reinterpret_cast<char*>(ps) + sizeof(PlayerStatus) * packet.playerNum - remain, 
					remain, 0);
				remain -= retval;
			}

			remain = sizeof(Coord) * packet.enemyNum;
			while (remain > 0) {
				retval = send(player.socket, reinterpret_cast<char*>(es) + sizeof(Coord) * packet.enemyNum - remain, 
					remain, 0);
				remain -= retval;
			}

			remain = sizeof(Coord) * packet.bulletNum;
			while (remain > 0) {
				retval = send(player.socket, reinterpret_cast<char*>(bs) + sizeof(Coord) * packet.bulletNum - remain, 
					remain, 0);
				remain -= retval;
			}
		}
#ifdef SERVER_DEBUG
		cout << "SC_PACKET_OBJECTS_INFO 관련 정보 추가 전송" << endl;
#endif // SERVER_DEBUG
	}
	else {
		sc_packet_music_end packet;
		packet.type = SC_PACKET_MUSIC_END;
		packet.size = sizeof(sc_packet_music_end);

		for (auto& player : players) {
			Send(player.socket, &packet);
		}
		musicStart = false;
		musicEnd = true;
	}

}

void SharedData::PlayerJoin(SOCKET socket, int selectMusic)
{
	g_selectedMusic = selectMusic;

	int newId = 0;
	for (int i = 0; i < MAX_USER; ++i) {
		bool isIdOccupied = false;
		for (auto& player : players) {
			if (player.ID == i) {
				isIdOccupied = true;
				break;
			}
		}
		if (!isIdOccupied)
		{
			newId = i;
			break;
		}

	}
	std::cout << "새 ID 는 " << newId << "입니다." << std::endl;

	players.emplace_back(PlayerData{ socket, newId });

	sc_packet_login_confirm packet;
	packet.type = SC_PACKET_LOGIN_CONFIRM;
	packet.size = sizeof(sc_packet_login_confirm);
	packet.playerID = newId;
	Send(socket, &packet);
#ifdef SERVER_DEBUG
	cout << "SC_PACKET_LOGIN_CONFIRM 전송" << endl;
#endif // SERVER_DEBUG
}

void SharedData::PlayerLeft(SOCKET sock) 
{
	players.erase(remove_if(players.begin(), players.end(), [=](const PlayerData& a) {
		return a.socket == sock;
		}), players.end());

}

void SharedData::CreateNewGame()
{
	if (musicStart) return;
	musicStart = true;

	memset(&musicSpeed, 0x00, sizeof(int) * 3000);
	memset(&musicNote, 0x00, sizeof(int) * 3000 * 12);

	char Inbuff[3000];
	DWORD read_size = 3000;
	DWORD c = 3000;

	switch (g_selectedMusic) {
	case 0:
		musicSpeedFile = CreateFile(L"Data\\bbkkbkkSpeed.txt",
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		timeDelay = -0.6f;
		break;
	case 1:
		musicSpeedFile = CreateFile(L"Data\\TrueBlueSpeed.txt",
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		break;
	}
	ReadFile(musicSpeedFile, Inbuff, c, &read_size, NULL);
	CloseHandle(musicSpeedFile);

	int num = 0;

	for (int i = 0; i < 3000; ++i) {
		if (Inbuff[i] >= 48 && Inbuff[i] <= 57) {
			musicSpeed[num] = Inbuff[i] - 48;
			++num;
		}
	}

	char noteInbuff[36000];
	DWORD read_note = 36000;
	DWORD n = 36000;

	switch (g_selectedMusic) {
	case 0:
		musicNoteFile = CreateFile(L"Data\\bbkkbkkNote.txt", 
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		break;
	case 1:
		musicNoteFile = CreateFile(L"Data\\TrueBlueNote.txt", 
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		break;
	}
	ReadFile(musicNoteFile, noteInbuff, n, &read_note, NULL);
	CloseHandle(musicNoteFile);

	int numI = 0, numJ = 0;
	for (int i = 0; i < 36000; ++i) {
		if (noteInbuff[i] >= 48 && noteInbuff[i] <= 57) {
			musicNote[numI][numJ] = noteInbuff[i] - 48;
			if (numJ == 11) {
				numJ = 0, numI++;
			}
			else {
				numJ++;
			}
		}
	}

	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		enemys.emplace_back(Enemy(i));
	}

	for (auto& player : players) {
		sc_packet_start_game packet;
		packet.type = SC_PACKET_START_GAME;
		packet.size = sizeof(sc_packet_start_game);
		packet.playerNum = players.size();
		Send(player.socket, &packet);
	}
#ifdef SERVER_DEBUG
	cout << "SC_PACKET_START_GAME 전송" << endl;
#endif // SERVER_DEBUG
}

void SharedData::ServerReset()
{
	musicStart = false;
	musicEnd = false;
	tick = 0;
	timeDelay = -1.f;

	players.clear();
	for (auto& enemy : enemys) {
		enemy.GetBullets().clear();
	}
	enemys.clear();
}

PlayerData& SharedData::GetPlayerData(int playerId)
{
	for (auto& player : players) {
		if (player.ID == playerId) {
			return player;
		}
	}
	return players[0];
}

int SharedData::GetBulletNum()
{
	int count = 0;
	for (auto& enemy : enemys) {
		count += enemy.GetBulletCount();
	}
	return count;
}
