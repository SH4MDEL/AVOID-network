#include "PlayerData.h"

CPlayerData::CPlayerData() {
	playerSocket = NULL;
	playerState = PLAYER_STATE::NONE;
	selectedMusic = SELECTED_MUSIC::NONE;
	playerId = 0;
	position.x = 0;
	position.y = 0;
	isSkill = FALSE;
}

CPlayerData::CPlayerData(SOCKET sock, PLAYER_STATE state, SELECTED_MUSIC music, int id) {
	playerSocket = sock;
	playerState = state;
	selectedMusic = music;
	playerId = id;
	position.x = 0;
	position.y = 0;
	isSkill = FALSE;
}

ServerSharedData::ServerSharedData() {
	m_pPlayers.clear();

	m_pEnemies.clear();
}

void ServerSharedData::PlayerJoin(SOCKET sock, char* dataBuf) {
	int* playerId = reinterpret_cast<int*>(dataBuf);
	
}

void ServerSharedData::PlayerLeft(int playerId) {
	
}

void ServerSharedData::CreateNewGame() {
	
}
