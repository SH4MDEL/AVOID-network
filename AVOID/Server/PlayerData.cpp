#include "PlayerData.h"

CPlayerData::CPlayerData() {
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
	m_pPlayers.clear();

	m_pEnemies.clear();
	time = 0;
	TimeDelay = -3.2f;
	nextPacket = NULL;
	nextPacketPlayerId = NULL;
	
}

void ServerSharedData::PlayerJoin(SOCKET sock, char* dataBuf) {
	char* selected = reinterpret_cast<char*>(dataBuf);

	if (selected == nullptr) {
		std::cout << "Error in PlayerJoin. selected is nullptr." << std::endl;
		return;
	}

	char num = (*selected);

	if (num != 0 && num != 1)
	{
		std::cout << "Error in PlayerJoin" << std::endl;
	}

	bool occupiedId[3] = { FALSE };

	for (auto& i : m_pPlayers)
	{
		occupiedId[i.playerId] = TRUE;
	}

	int newId = NULL;

	for (int i = 0; i < MAX_USER; ++i) {
		if (!occupiedId[i]) {
			newId = i;
			break;
		}
	}

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

	nextPacket = SC_PACKET_LOGIN_CONFIRM;
	nextPacketPlayerId = newId;
}

void ServerSharedData::PlayerLeft(char* dataBuf) {
	int* leftPlayerId = reinterpret_cast<int*>(dataBuf);

	// 유저 데이터 삭제
	m_pPlayers.erase(remove_if(m_pPlayers.begin(), m_pPlayers.end(), [=](const CPlayerData& a) {return a.playerId == (*leftPlayerId); }), m_pPlayers.end());

	// 그리고 쓰레드 아이디도 저장해 두는 것이 좋을 수도 있을 듯
	// 
	nextPacket = SC_PACKET_LOGOUT;
	nextPacketPlayerId = NO_NEED_PLAYER_ID;
}

void ServerSharedData::ChangePlayerState(PLAYER_STATE state)
{
	// 일단은 비워둠.
}

void ServerSharedData::UpdatePlayerStatus(SOCKET sock, char* dataBuf)
{
	PlayerStatusByPacket* packet = reinterpret_cast<PlayerStatusByPacket*>(dataBuf);

	for (auto& i : m_pPlayers) {
		if (packet->playerID == i.playerId)
		{
			i.position = packet->position;
			i.isSkill = packet->isSkill;
			i.isStatusChanged = TRUE;
			break;
		}
	}



	// 모든 플레이어에게서 스테이터스를 받으면 충돌 체크 스레드를 생성해 충돌처리를 실행한다.
	if (CheckAllPlayerStatusReceived()) {
		ResetEvent(hClientEvent);
		SetEvent(hCollideEvent);
		WaitForSingleObject(hClientEvent, INFINITE);
	}
	else {
		nextPacket = NULL;
		nextPacketPlayerId = NULL;
	}

}



// 모든 플레이어가 다 한번 스테이터스를 바꿨는지 확인.
bool ServerSharedData::CheckAllPlayerStatusReceived() {
	for (auto& i : m_pPlayers) {
		if (!(i.isStatusChanged)) {
			return false;
		}
	}

	return true;
}

void ServerSharedData::CreateNewGame(char* dataBuf) {

	int musicNum = 0;


	if (dataBuf == NULL) {

		musicNum = 0;

	}
	else {

		char data = NULL;
		memcpy(&data, dataBuf, sizeof(char));

		for (auto& player : m_pPlayers) {
			if (player.playerId == data) {
				switch (player.selectedMusic) {
				case SELECTED_MUSIC::BBKKBKK: 
				{
					int musicNum = 0;
					music = SELECTED_MUSIC::BBKKBKK;
				}
				break;
				case SELECTED_MUSIC::TRUE_BLUE:
				{
					int musicNum = 1;
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
	
	switch(musicNum){
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
	nextPacketPlayerId = NULL;
}

int ServerSharedData::GetPlayerRank(SOCKET sock, char* dataBuf) {
	
	char Id = NULL;
	memcpy(&Id, dataBuf, sizeof(char));

	char hp = NULL;
	memcpy(&Id, dataBuf + sizeof(char), sizeof(char));

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
				leastTime = 0;
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
				leastTime = 0;
			}
		}	
	}
	for (auto& enemy : m_pEnemies) {
		enemy.Update(leastTime);
	}
	if ((music == SELECTED_MUSIC::BBKKBKK && time >= 1450) && (music == SELECTED_MUSIC::TRUE_BLUE && time >= 1310)) {
		nextPacket = SC_PACKET_MUSIC_END;
		nextPacketPlayerId = NO_NEED_PLAYER_ID;
	}
	else {
		nextPacket = SC_PACKET_OBJECTS_INFO;
		nextPacketPlayerId = NO_NEED_PLAYER_ID;
	}

}
