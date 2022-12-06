#pragma once
#include "stdafx.h"
#include "Protocol.h"
#include "Obj_Enemy.h"

constexpr int NO_NEED_PLAYER_ID = -1;

extern HANDLE hClientEvent;
extern HANDLE hCollideEvent;

extern DWORD WINAPI Collision_Thread(LPVOID arg);

constexpr int PLAYER_RADIUS = 10;
constexpr int ABILITY_RADIUS = 85;

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
	HANDLE hPlayingMusicSpeedFile, hPlayingMusicNoteFile;
	SELECTED_MUSIC music = SELECTED_MUSIC::NONE;

	bool isCollideOk = false;

	int lotateSpeed[3000] = { NULL };
	int note[3000][12] = { 0 };
	int time = 0;
	float TimeDelay = -3.f;
	float leastTime = 0;
	bool musicStart = 0;
	char nextPacket;
	int nextPacketPlayerId;

	
	ServerSharedData();
	void PlayerJoin(SOCKET sock, char* dataBuf);
	void PlayerLeft(SOCKET sock);
	void UpdatePlayerStatus(SOCKET sock, char* dataBuf);
	int GetPlayerRank(SOCKET sock, char* dataBuf);
	bool CheckAllPlayerStatusReady();
	void CreateNewGame(char* dataBuf);
	void Update(float fTimeElapsed);
	int GetBulletNum();
};


class SelectedMusicByPacket {
	char playerID;
};

class PlayerStatusByPacket {
public:
	Coord position;
	char playerID;
	bool isSkill;

};