#include "PlayerData.h"

CPlayerData::CPlayerData() {

	// �÷��̾� �����͸� �� �޾��� �� �ʱ�ȭ

	playerSocket = NULL;
	playerState = PLAYER_STATE::NONE;
	selectedMusic = SELECTED_MUSIC::NONE;
	playerId = 0;
	position.x = 0;
	position.y = 0;
	isSkill = FALSE;
	isStatusChanged = FALSE;
	isInvincible = FALSE;
	isCollide = FALSE;
	AttackedTime = 0.0f;
	hp = -1;

}

CPlayerData::CPlayerData(SOCKET sock, PLAYER_STATE state, SELECTED_MUSIC music, int id) {

	// �÷��̾� �����͸� �޾��� �� �ʱ�ȭ

	playerSocket = sock;
	playerState = state;
	selectedMusic = music;
	playerId = id;
	position.x = 0;
	position.y = 0;
	isSkill = FALSE;
	isStatusChanged = FALSE;
	isInvincible = FALSE;
	isCollide = FALSE;
	AttackedTime = 0.0f;
	hp = -1;

}

ServerSharedData::ServerSharedData() {

	// ���ʷ� ������ �����Ǿ��� ��


	m_pPlayers.clear();
	m_pEnemies.clear();

	time = 0;
	TimeDelay = -0.8f;
	nextPacket = NULL;
	nextPacketPlayerId = NULL;
	musicStart = false;
}

void ServerSharedData::PlayerJoin(SOCKET sock, char* dataBuf) {
	
	// �÷��̾ PlayerWaiting Scene�� ������ �� ����
	char* selected = reinterpret_cast<char*>(dataBuf);

	// selected�� �������͸� ������.
	if (selected == nullptr) {
		return;
	}

	// ���� ���� ���� ��ȣ�� ����
	char num = (*selected);

	// ���� ���� ��ȣ�� 0 �Ǵ� 1 ��, ������ �ִ� ������ �ּҰ� �ƴ� ��� ����
	if (num != 0 && num != 1)
	{
		std::cout << "Error in PlayerJoin" << std::endl;
		return;
	}

	// ID ����
	int newId = 0;

	for (int i = 0; i < MAX_USER; ++i) {

		bool isIdOccupied = false;

		for (auto& player : m_pPlayers) {
			if (player.playerId == i) {
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

	std::cout << "�� ID �� " << newId << "�Դϴ�." << std::endl;
	// �÷��̾� ������ ����

	CPlayerData newPlayer;

	if (num == 0)
	{
		newPlayer = CPlayerData(sock, PLAYER_STATE::PLAYER_WAITING, SELECTED_MUSIC::BBKKBKK, newId);
		
	}
	else if (num == 1)
	{
		newPlayer = CPlayerData(sock, PLAYER_STATE::PLAYER_WAITING, SELECTED_MUSIC::TRUE_BLUE, newId);
	}

	m_pPlayers.push_back(newPlayer);

	// ������ ���� ��Ŷ ����

	nextPacket = SC_PACKET_LOGIN_CONFIRM;
	nextPacketPlayerId = newId;
}

void ServerSharedData::PlayerLeft(SOCKET sock) {

	//�÷��̾ ��������� �ľ����� ���� ����

	// �ش� ���Ͽ� �ش��ϴ� �÷��̾� �����͸� �����
	// ������ UINT_PTR Ÿ���̹Ƿ�, �� ������ ���� �� ����
	m_pPlayers.erase(remove_if(m_pPlayers.begin(), m_pPlayers.end(), [=](const CPlayerData& a) {
		return a.playerSocket == sock;
		}), m_pPlayers.end());

}


void ServerSharedData::UpdatePlayerStatus(SOCKET sock, char* dataBuf)
{

	// �����͸� �޾��� ��

	PlayerStatusByPacket* packet = reinterpret_cast<PlayerStatusByPacket*>(dataBuf);

	for (auto& player : m_pPlayers) {
		if (packet->playerID == player.playerId)
		{
			player.position = packet->position;
			player.isSkill = packet->isSkill;
			player.isStatusChanged = TRUE;
			std::cout << player.playerId << "�� �÷��̾� ���� ���� ������ ����" << std::endl;
			break;
		}
	}

	// ��� �÷��̾�Լ� �������ͽ��� ������ �浹 üũ �����带 ������ �浹ó���� �����Ѵ�.
	// while�� ����ؼ� Context Switching�� �Ͼ�� �ǹǷ� ���� ����. �׷��� ������ �ذ�å�� �������� �����Ƿ� �ϴ��� while�� ����д�.

	// ������ �˰� �Ǿ���. �̹� CriticalSection�� �� �����Ƿ� ������ ù �����忡���� ���� �ȴ�.
	// �׷��ϱ� Apply �ܰ迡���� �����͸� �����ϴ� �͸� �ϴ� ���� �´� ������ ���δ�.
	// �׸��� �����带 ���� ���� �ش� �浹üũ�� �ϴ� ���� �´��� Ȯ���Ͽ� �浹üũ�� ������ ���°��� �´µ� �ϴ�.
	// �׷��ϱ� �׻� �����带 ����� ������ �ؾ߰ڴ�.

	nextPacket = SC_PACKET_OBJECTS_INFO;
	nextPacketPlayerId = packet->playerID;

	ResetEvent(hClientEvent);
	SetEvent(hCollideEvent);
	CreateThread(NULL, 0, Collision_Thread, NULL, 0, NULL);

}

// ��� �÷��̾ �� �ѹ� �������ͽ��� �ٲ���� Ȯ��.
bool ServerSharedData::CheckAllPlayerStatusReady() {
	for (auto& i : m_pPlayers) {
		if (!(i.isStatusChanged)) {
			return false;
		}
	}

	std::cout << "�浹 üũ �̺�Ʈ ���� ����" << std::endl;
	for (auto& i : m_pPlayers) {
		i.isStatusChanged = false;
	}
	return true;
}

void ServerSharedData::CreateNewGame(char* dataBuf) {

	int musicNum = 0;

	if (!musicStart) {
		

		if (dataBuf == NULL) {

			musicNum = 0;

		}
		else {
			char data = (*dataBuf);

			for (auto& player : m_pPlayers) {
				if (player.playerId == data) {
					switch (player.selectedMusic) {
					case SELECTED_MUSIC::BBKKBKK:
					{
						musicNum = 0;
						music = SELECTED_MUSIC::BBKKBKK;
					}
					break;
					case SELECTED_MUSIC::TRUE_BLUE:
					{
						musicNum = 1;
						music = SELECTED_MUSIC::TRUE_BLUE;
					}
					break;
					}
				}
			}
		}

		char Inbuff[3000];
		DWORD read_size = 3000;
		DWORD c = 3000;

		switch (musicNum) {
		case 0:
			hPlayingMusicSpeedFile = CreateFile(L"Data\\bbkkbkkSpeed.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			break;
		case 1:
			hPlayingMusicSpeedFile = CreateFile(L"Data\\TrueBlueSpeed.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			break;
		}
		ReadFile(hPlayingMusicSpeedFile, Inbuff, c, &read_size, NULL);
		CloseHandle(hPlayingMusicSpeedFile);

		int num = 0;

		for (int i = 0; i < 3000; ++i) {
			if (Inbuff[i] >= 48 && Inbuff[i] <= 57) {
				lotateSpeed[num] = Inbuff[i] - 48;
				++num;
			}
		}

		char noteInbuff[36000];
		DWORD read_note = 36000;
		DWORD n = 36000;

		switch (musicNum) {
		case 0:
			hPlayingMusicNoteFile = CreateFile(L"Data\\bbkkbkkNote.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			break;
		case 1:
			hPlayingMusicNoteFile = CreateFile(L"Data\\TrueBlueNote.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			break;
		}
		ReadFile(hPlayingMusicNoteFile, noteInbuff, n, &read_note, NULL);
		CloseHandle(hPlayingMusicNoteFile);

		int numI = 0, numJ = 0;

		for (int i = 0; i < 36000; ++i) {
			if (noteInbuff[i] >= 48 && noteInbuff[i] <= 57) {
				note[numI][numJ] = noteInbuff[i] - 48;
				if (numJ == 11) {
					numJ = 0, numI++;
				}
				else {
					numJ++;
				}
			}
		}

		for (int i = 0; i < NUMBER_OF_ENEMY; ++i)
		{
			m_pEnemies.emplace_back(Enemy(i));
		}

		nextPacket = SC_PACKET_START_GAME;
		nextPacketPlayerId = NO_NEED_PLAYER_ID;

		musicStart = true;
	}
}

int ServerSharedData::GetPlayerRank(SOCKET sock, char* dataBuf) {
	
	char Id = dataBuf[0];
	char hp = dataBuf[1];

	for (auto& player : m_pPlayers) {
		if (player.playerId == Id) {
			player.hp = hp;
		}
	}

	for (auto& player : m_pPlayers) {
		if (player.hp == -1) {
			nextPacket = NULL;
			nextPacketPlayerId = NULL;
			return -1;
		}
	}


	std::sort(m_pPlayers.begin(), m_pPlayers.end(), [](const CPlayerData& a, const CPlayerData& b) {
		return a.hp > b.hp;
		});


	nextPacket = SC_PACKET_RANK;
	nextPacketPlayerId = NULL;

	return 0;
}


int ServerSharedData::GetBulletNum() {
	
	int sum = 0;
	for (auto& enemy : m_pEnemies) {
		sum += enemy.GetBullets().size();
	}

	return sum;
}

void ServerSharedData::Update(float fTimeElapsed) {

	TimeDelay += fTimeElapsed;
	if (TimeDelay >= 0.f) {
		leastTime += fTimeElapsed;

		if (music == SELECTED_MUSIC::BBKKBKK) {
			if (TimeDelay >= (60.f / 680.f) * (float)time) {
				int i = 0;
				for (auto& enemy : m_pEnemies) {
					enemy.SetNextNote(leastTime, lotateSpeed[time], note[time][i], 0);
					++i;
				}
				++time;
				leastTime = 0.f;
			}
		}
		else if (music == SELECTED_MUSIC::TRUE_BLUE) {
			if (TimeDelay >= (60.f / 656.f) * (float)time) {
				int i = 0;
				for (auto& enemy : m_pEnemies) {
					enemy.SetNextNote(leastTime, lotateSpeed[time], note[time][i], 1);
					++i;
				}
				++time;
				leastTime = 0.f;
			}
		}	
	}

	for (auto& player : m_pPlayers) {
		if (player.isInvincible) {
			if (TimeDelay - player.AttackedTime > 2.0f) {
				player.isInvincible = false;
			}
		}
	}

	for (auto& enemy : m_pEnemies) {
		enemy.Update(fTimeElapsed);
	}
	

	if ((music == SELECTED_MUSIC::BBKKBKK && time >= 1450) || (music == SELECTED_MUSIC::TRUE_BLUE && time >= 1310)) {
		nextPacket = SC_PACKET_MUSIC_END;
		nextPacketPlayerId = NO_NEED_PLAYER_ID;
		musicStart = false;
	}
	else {
		nextPacket = SC_PACKET_OBJECTS_INFO;
		nextPacketPlayerId = NO_NEED_PLAYER_ID;
	}

}
