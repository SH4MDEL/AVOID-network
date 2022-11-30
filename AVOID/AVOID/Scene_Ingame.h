#pragma once
#include "Scene.h"

class Sound;
class OBJECT_MainEnemy;
class OBJECT_Bullet;
class OBJECT_Player;

extern int finalhp;

class Scene_Ingame : public CScene
{
public:
	Scene_Ingame();
	Scene_Ingame(SceneTag tag, CFramework* pFramework);
	~Scene_Ingame();

	void OnDestroy();
	void OnCreate() override;		// 만들때 선언용 함수

	void BuildObjects() override;
	void Render(HDC hdc) override;
	void Update(float fTimeElapsed) override;

	void PlayerCrash(OBJECT_MainEnemy* Enemy);
	void AbilityCrash(OBJECT_MainEnemy* Enemy);

	void SetPlayerNum(int playerNum);
	void SetPlayerID(int playerID);
	void SetMusic(int selectedMusic);

	void KeyState();

	// 서버 관련 추가 함수
#ifdef USE_NETWORK
	void SetPlayerEnemyData(char playerNum, char enemyNum, char bulletNum) 
	{ m_playerNum = playerNum, m_enemyNum = enemyNum, m_bulletNum = bulletNum; }
	array<PlayerStatus, 3>& GetPlayersCoord() { return m_playersStatus; }
	array<Coord, 12>& GetEnemysCoord() { return m_enemysCoord; }
	array<Coord, 50>& GetBulletsCoord() { return m_bulletsCoord; }

	void SetRank(char rank) { m_rank = (int)rank; }
#endif
private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	OBJECT_MainEnemy* CMainEnemy[12] = { NULL };
	OBJECT_Player* m_players[3];

	Sound* IngameSound = NULL;

	CImage Ingame;			// 배경 화면
	CImage Circle;			// 플레이가 이뤄질 원	

	int Circleradius = 200;

	int m_selectedMusic;
	float TimeDelay = -3;
	float leastTime = 0;
	bool musicStart = 0;

	HANDLE hFileSpeed, hFileNote;

	int lotateSpeed[3000] = { NULL };
	int note[3000][12] = { 0 };				// 파일 입출력을 통하여 노트의 정보를 받아와 저장한다.
	int time = 0;							// 노래가 시작하고 얼마나 지나갔는가? 1당 bpm / 240 이다.
											// time 이 1450을 넘으면 노래 종료

	// 서버 관련 추가 변수
	// recv
#ifdef USE_NETWORK
	int						m_playerID;
	int						m_playerNum;
	int						m_enemyNum;
	int						m_bulletNum;
	array<PlayerStatus, 3>	m_playersStatus;
	array<Coord, 12>		m_enemysCoord;
	array<Coord, 50>		m_bulletsCoord;
	int						m_rank;

	OBJECT_Bullet*			m_bullets[50];
#endif
	// send
};																



