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
	char* selected = reinterpret_cast<char*>(dataBuf);

	CPlayerData newPlayer;

	if (*selected != 1 || *selected != 2)
	{
		std::cout << "Error in PlayerJoin" << std::endl;
	}

	if (*selected == 1)
	{
		newPlayer = CPlayerData(sock, PLAYER_STATE::PLAYER_WAITING, SELECTED_MUSIC::BBKKBKK, 0);
	}
	else if (*selected == 2)
	{
		newPlayer = CPlayerData(sock, PLAYER_STATE::PLAYER_WAITING, SELECTED_MUSIC::TRUE_BLUE, 0);
	}

	m_pPlayers.push_back(newPlayer);
	
	

}

void ServerSharedData::PlayerLeft(char* dataBuf) {
	int* leftPlayerId = reinterpret_cast<int*>(dataBuf);

	// 유저 데이터 삭제
	m_pPlayers.erase(remove_if(m_pPlayers.begin(), m_pPlayers.end(), [=](const CPlayerData& a) {return a.playerId == (*leftPlayerId); }), m_pPlayers.end());

	// 그리고 쓰레드 아이디도 저장해 두는 것이 좋을 수도 있을 듯
	// 

}

void ServerSharedData::ChangePlayerState(PLAYER_STATE state)
{

}

void ServerSharedData::UpdatePlayerStatus(SOCKET sock, char* dataBuf)
{
	PlayerStatusByPacket* packet = reinterpret_cast<PlayerStatusByPacket*>(dataBuf);

	for (auto& i : m_pPlayers) {
		if (packet->playerID == i.playerId)
		{
			i.position = packet->position;
			i.isSkill = packet->isSkill;
			break;
		}
	}

}

void ServerSharedData::CreateNewGame() {
	
}
