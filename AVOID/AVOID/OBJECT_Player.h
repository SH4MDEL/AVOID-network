#pragma once
#include "Object.h"

extern int mouse_mx;
extern int mouse_my;

class OBJECT_Player : public CObject
{
public:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	OBJECT_Player();
	OBJECT_Player(int x, int y);						//
	OBJECT_Player(int x, int y, int playerID);
	~OBJECT_Player();

	CImage Player[3];
	CImage Ability;

	float hitHp = 10;									// 한번의 피격 시 10이 달도록 조정함
	int Ability_radius = 85;

	void OnCreate(int x, int y);
	void Update(float fTimeElapsed);
	void Render(HDC* hDC);

	int GetX();
	int GetY();
	float GetHp();
	float GetMp();
	bool GetState();				// 무적인지 상태를 받는 함수
	int GetRadius();				// 반지름 받는 함수
	bool GetAbilityState();
	int GetID();

	void SetHp(float hp);
	void SetHp_zero();
	void SetAbility(int ability);
	void SetState(bool invincibility);

	void SetPos(float x, float y) { m_x = x, m_y = y; };

	void KeyState();
	void MouseState();

	bool GetSpaceKeyDown() { return m_spaceKeyDown; }
	void SetAbilityState(bool useAbility) { m_abilityCheck = useAbility; }

	// 서버 관련 추가 함수입니다.
	int GetServerX() { return m_x - windowX / 2; }	
	int GetServerY() { return m_y - windowY / 2; }
	// 클라이언트의 기준 좌표계를 서버 좌표계로 변경합니다.
	void SetServerPos(float x, float y) 
	{ m_x = x + windowX / 2, m_y = y + windowY / 2; }
	// 전송된 서버 좌표계를 클라이언트 좌표계로 변경합니다.
#ifdef USE_NETWORK
	void SetPlayerID(int playerID) { m_playerID = playerID; }
#endif
private:
	float m_x = 0;											// x좌표
	float m_y = 0;											// y좌표
	float Player_hp = 30;									// 플레이어의 hp
	float hp_restore = 0;
	float Player_mp = 250;
	float mp_restore = 0;

	float TimeDelay = 0;

	bool invincibility = false;								// 피격시 무적 확인용 변수
	int Player_radius = 10;									// 플레이어 원의 반지름			 바꿀 시 Bullet의 특수기능 충돌체크의 반지름도 바꿔줘야함
	int Player_ability = 1;									// 특수능력	
	bool m_abilityCheck;								// 능력이 발동중인지 확인하는 변수
	bool m_spaceKeyDown;
	bool CheckKey = false;

	// 서버 관련 추가 변수입니다.
	int m_playerID;
};