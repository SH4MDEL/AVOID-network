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
	hp = 0;
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
	hp = 0;
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

	int num = (*selected);

	CPlayerData newPlayer;

	if (num != 1 || num != 2)
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

	if (num == 1)
	{
		newPlayer = CPlayerData(sock, PLAYER_STATE::PLAYER_WAITING, SELECTED_MUSIC::BBKKBKK, newId);
		
	}
	else if (num == 2)
	{
		newPlayer = CPlayerData(sock, PLAYER_STATE::PLAYER_WAITING, SELECTED_MUSIC::TRUE_BLUE, newId);
	}

	m_pPlayers.push_back(newPlayer);

	nextPacket = SC_PACKET_LOGIN_CONFIRM;
	nextPacketPlayerId = newPlayer.playerId;
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
		// �浹�� ���� ó���� �־�� �Ѵ�.
		// ���⼭ Set Event�� ����ϴ��� �ؾ���.
		// ���� �ڼ��� ������ �ʿ䰡 �ִ� ��.
		// �ϴ��� �浹 üũ �����带 �������� �Űܳ��� �������� ������ �غ���.

		// ���� ��� �÷��̾��� �������ͽ� ��ȭ Ȯ���� �ʱ�ȭ �� �־�� �Ѵ�.
	}

}

DWORD WINAPI Collision_Thread(LPVOID arg)
{
	return 0;
	// �浹 üũ �����带 ��� �Ű� ��.
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

void ServerSharedData::CreateNewGame() {
	fElapsedTime = 0.0f;
	
	for (int i = 0; i < NUMBER_OF_ENEMY; ++i)
	{
		m_pEnemies.push_back(Enemy(i));
	}
}

int ServerSharedData::GetPlayerRank(SOCKET sock, char* dataBuf) {
	//(*dataBuf); // �ش� �����ʹ� hp�ε�, hp�� ��� rand�� �ٲ�� �ϴ°�
				// -> SOCKET�� �񱳰� �����Ѱ�
				// -> �÷��̾� ID�� �������� ������ ���� ������ ���� �� ��������
	return 0;
}

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

void SendPacket() {

}

int ServerSharedData::GetBulletNum() {
	
	int sum = 0;
	for (auto& i : m_pEnemies) {
		sum += i.m_bullets.size();
	}

	return sum;
}
