#include "stdafx.h"
#include "Protocol.h"
#include "Obj_Enemy.h"

constexpr int NO_NEED_PLAYER_ID = -1;

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
	bool isStatusChanged;
	bool isInvincible;
	bool isCollide;
	float AttackedTime;
	int hp;

	CPlayerData();
	CPlayerData(SOCKET sock, PLAYER_STATE state, SELECTED_MUSIC music, int id);
};

class ServerSharedData {
public:
	std::vector<CPlayerData> m_pPlayers;
	std::vector<Enemy> m_pEnemies;
	float fElapsedTime;
	int playerIdHandle;
	char nextPacket;
	int nextPacketPlayerId;

	
	ServerSharedData();
	void PlayerJoin(SOCKET sock, char* dataBuf);
	void PlayerLeft(char* dataBuf);
	void ChangePlayerState(PLAYER_STATE state);
	void UpdatePlayerStatus(SOCKET sock, char* dataBuf);
	bool CheckAllPlayerStatusReceived();
	int GetPlayerRank(SOCKET sock, char* dataBuf);
	void CreateNewGame();
	void MakePacket(char packetType, int playerId);
	int GetBulletNum();
	void SendPacket();
};


class PlayerStatusByPacket {
public:
	Coord position;
	char playerID;
	bool isSkill;

};