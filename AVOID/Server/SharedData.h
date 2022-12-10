#pragma once
#include "stdafx.h"
#include "main.h"
#include "Enemy.h"

constexpr int PLAYER_RADIUS = 10;
constexpr int ABILITY_RADIUS = 85;

extern queue<PlayerStatus>	g_insertPlayerStatus;
extern vector<int>			g_hpData;
extern mutex				g_mutex;

struct PlayerData
{
	PlayerData(SOCKET socket, int ID) : socket(socket), ID(ID), position(Coord{0, 0}), 
	isSkill(false), isInvincible(false), isCollide(false), invincibleTime(0.f), Hp(0){}

	SOCKET socket;
	int ID;
	Coord position;
	bool isSkill;
	bool isInvincible;
	bool isCollide;
	float invincibleTime;
	int Hp;

};

struct SharedData
{
	SharedData();

	void Update(float timeElapsed);

	void PlayerJoin(SOCKET socket, int selectMusic);
	void PlayerLeft(SOCKET sock);
	void CreateNewGame();
	void ServerReset();

	PlayerData& GetPlayerData(int playerId);
	int GetBulletNum();

	vector<PlayerData>	players;
	vector<Enemy>		enemys;

	bool				musicStart;
	bool				musicEnd;
	HANDLE				musicSpeedFile, musicNoteFile;
	int					musicSpeed[3000];
	int					musicNote[3000][12];
	int					tick;
	float				timeDelay;


};

