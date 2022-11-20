#include "stdafx.h"
#include "Protocol.h"
#include "Obj_Enemy.h"

enum class PLAYER_STATE{
	NONE,
	PLAYER_WAITING,
	PLAY_GAME,
	WAIT_RESULT
};

enum class SELECTED_MUSIC {
	NONE,
	BBKKBKK,
	TRUE_BLUE
};

class CPlayerData {
public:
	SOCKET playerSocket;
	PLAYER_STATE playerState;
	SELECTED_MUSIC selectedMusic;
	int playerId;
	Coord position;
	bool isSkill;

	CPlayerData();
	CPlayerData(SOCKET sock, PLAYER_STATE state, SELECTED_MUSIC music, int id);
};

class ServerSharedData {
public:
	std::vector<CPlayerData> m_pPlayers;
	std::vector<Enemy> m_pEnemies;
	float fElapsedTime;

	
	ServerSharedData();
	void PlayerJoin(SOCKET sock, char* dataBuf);
	void PlayerLeft(char* dataBuf);
	void ChangePlayerState(PLAYER_STATE state);
	void UpdatePlayerStatus(SOCKET sock, char* dataBuf);
	int GetPlayerRank(SOCKET sock, char* dataBuf);
	void CreateNewGame();

};


class PlayerStatusByPacket {
private:
	Coord position;
	char playerID;
	bool isSkill;
public:
	PlayerStatusByPacket();
	PlayerStatusByPacket(short x, short y, char playerID, bool isSkill);
};