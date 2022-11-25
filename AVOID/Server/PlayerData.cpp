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
		// 충돌을 위한 처리가 있어야 한다.
		// 여기서 Set Event를 사용하던지 해야함.
		// 좀더 자세히 공부할 필요가 있는 듯.
		// 일단은 충돌 체크 스레드를 이쪽으로 옮겨놓고 여러가지 실험을 해보자.

		// 이후 모든 플레이어의 스테이터스 변화 확인을 초기화 해 주어야 한다.
	}

}

DWORD WINAPI Collision_Thread(LPVOID arg)
{
	return 0;
	// 충돌 체크 스레드를 잠깐 옮겨 둠.
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

void ServerSharedData::CreateNewGame() {
	fElapsedTime = 0.0f;
	
	for (int i = 0; i < NUMBER_OF_ENEMY; ++i)
	{
		m_pEnemies.push_back(Enemy(i));
	}
}

int ServerSharedData::GetPlayerRank(SOCKET sock, char* dataBuf) {
	//(*dataBuf); // 해당 데이터는 hp인데, hp를 어떻게 rand로 바꿔야 하는가
				// -> SOCKET의 비교가 가능한가
				// -> 플레이어 ID를 보내주지 않으면 여러 문제가 있을 수 있을지도
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
