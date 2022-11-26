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
	fElapsedTime = 0.0f;
	nextPacket = NULL;
	nextPacketPlayerId = NULL;
	playerIdHandle = 0;
	
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

	// ���� ������ ����
	m_pPlayers.erase(remove_if(m_pPlayers.begin(), m_pPlayers.end(), [=](const CPlayerData& a) {return a.playerId == (*leftPlayerId); }), m_pPlayers.end());

	// �׸��� ������ ���̵� ������ �δ� ���� ���� ���� ���� ��
	// 
	nextPacket = SC_PACKET_LOGOUT;
	nextPacketPlayerId = NO_NEED_PLAYER_ID;
}

void ServerSharedData::ChangePlayerState(PLAYER_STATE state)
{
	// �ϴ��� �����.
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


	// ��� �÷��̾�Լ� �������ͽ��� ������ �浹 üũ �����带 ������ �浹ó���� �����Ѵ�.
	if (CheckAllPlayerStatusReceived()) {
		ResetEvent(hClientEvent);
		SetEvent(hCollideEvent);
	}
	else {
		nextPacket = NULL;
		nextPacketPlayerId = NULL;
	}

}



// ��� �÷��̾ �� �ѹ� �������ͽ��� �ٲ���� Ȯ��.
bool ServerSharedData::CheckAllPlayerStatusReceived() {
	for (auto& i : m_pPlayers) {
		if (!(i.isStatusChanged)) {
			return false;
		}
	}

	return true;
}

void ServerSharedData::CreateNewGame(char* dataBuf) {
	fElapsedTime = 0.0f;

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
				}
				break;
				case SELECTED_MUSIC::TRUE_BLUE:
				{
					int musicNum = 1;
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
		hPlayingMusicSpeedFile = CreateFile(L"Data\\bbkkbkkSpeed.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
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
		if (noteInbuff[i] > 48 && noteInbuff[i] <= 57) {
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
		m_pEnemies.push_back(Enemy(i));
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

/*
void ServerSharedData::MakePacket(char packetType, int playerId) {
	switch (packetType) {
	case SC_PACKET_LOGIN_CONFIRM:
	{
		sc_packet_login_confirm packet;
		packet.playerID = playerId;
		packet.size = sizeof(sc_packet_login_confirm);
		packet.type = SC_PACKET_LOGIN_CONFIRM;
	}
		break;
	case SC_PACKET_START_GAME:
	{
		sc_packet_start_game packet;
		packet.playerNum = m_pPlayers.size();
		packet.size = sizeof(sc_packet_start_game);
		packet.type = SC_PACKET_START_GAME;
		
	}
	break;
	case SC_PACKET_OBJECTS_INFO:
	{
		sc_packet_objects_info packet;
		packet.bulletNum = GetBulletNum();
		packet.enemyNum = m_pEnemies.size();
		packet.playerNum = m_pPlayers.size();
		packet.size = sizeof(sc_packet_objects_info);
		packet.type = SC_PACKET_OBJECTS_INFO;
		
	}
	}
}
*/


int ServerSharedData::GetBulletNum() {
	
	int sum = 0;
	for (auto& enemy : m_pEnemies) {
		sum += enemy.GetBullets().size();
	}

	return sum;
}

void ServerSharedData::Update() {

}
