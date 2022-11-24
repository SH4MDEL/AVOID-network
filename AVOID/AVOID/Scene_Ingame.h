#pragma once
#include "Scene.h"

class Sound;
class OBJECT_MainEnemy;
class OBJECT_Player;

extern int finalhp;

class Scene_Ingame : public CScene
{
public:
	Scene_Ingame();
	Scene_Ingame(SceneTag tag, CFramework* pFramework);
	~Scene_Ingame();

	void OnDestroy();
	void OnCreate() override;		// ���鶧 ����� �Լ�

	void BuildObjects() override;
	void Render(HDC hdc) override;
	void Update(float fTimeElapsed) override;

	void PlayerCrash(OBJECT_MainEnemy* Enemy);
	void AbilityCrash(OBJECT_MainEnemy* Enemy);

	void SetMusic(int selectedMusic);

	void KeyState();

	// ���� ���� �߰� �Լ�
#ifdef USE_NETWORK
	void SetPlayerEnemyData(char playerNum, char bulletNum) { m_playerNum = playerNum, m_bulletNum = bulletNum; }
	array<Coord, 3>& GetPlayersCoord() { return m_playersCoord; }
	array<Coord, 12>& GetEnemysCoord() { return m_enemysCoord; }
	array<Coord, 50>& GetBulletsCoord() { return m_bulletsCoord; }

	void SetRank(char rank) { m_rank = (int)rank; }
#endif
private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	OBJECT_MainEnemy* CMainEnemy[12] = { NULL };
	OBJECT_Player* Player = nullptr;

	Sound* IngameSound = NULL;

	CImage Ingame;			// ��� ȭ��
	CImage Circle;			// �÷��̰� �̷��� ��	

	int Circleradius = 200;

	int m_selectedMusic;
	float TimeDelay = -3;
	float leastTime = 0;
	bool musicStart = 0;

	HANDLE hFileSpeed, hFileNote;

	int lotateSpeed[3000] = { NULL };
	int note[3000][12] = { 0 };				// ���� ������� ���Ͽ� ��Ʈ�� ������ �޾ƿ� �����Ѵ�.
	int time = 0;							// �뷡�� �����ϰ� �󸶳� �������°�? 1�� bpm / 240 �̴�.
											// time �� 1450�� ������ �뷡 ����

	// ���� ���� �߰� ����
	// recv
#ifdef USE_NETWORK
	int					m_playerID;
	int					m_playerNum;
	int					m_bulletNum;
	array<Coord, 3>		m_playersCoord;
	array<Coord, 12>	m_enemysCoord;
	array<Coord, 50>	m_bulletsCoord;
	int					m_rank;
#endif
	// send

};																



