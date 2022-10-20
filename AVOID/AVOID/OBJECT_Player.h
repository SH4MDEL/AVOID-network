#pragma once
#include "Object.h"

extern int mouse_mx;
extern int mouse_my;

//int finalhp = 0;

class OBJECT_Player : public CObject
{
private:
	float x = 0;											// x좌표
	float y = 0;											// y좌표
	float Player_hp = 30;									// 플레이어의 hp
	float hp_restore = 0;
	float Player_mp = 250;
	float mp_restore = 0;

	float TimeDelay = 0;

	bool invincibility = false;								// 피격시 무적 확인용 변수
	int Player_radius = 10;									// 플레이어 원의 반지름			 바꿀 시 Bullet의 특수기능 충돌체크의 반지름도 바꿔줘야함
	int Player_ability = 1;									// 특수능력	
	bool AbilityCheck = true;								// 능력이 발동중인지 확인하는 변수
	bool CheckKey = false;

	HBRUSH hBrush, oldBrush;

public:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	OBJECT_Player();
	OBJECT_Player(int x, int y);						//
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
	bool GetState();				// 무적인지 상태를 받는 함수
	int GetRadius();				// 반지름 받는 함수
	bool GetAState();

	void SetHp(float hp);
	void SetHp_zero();
	void SetAbility(int ability);
	void SetState(bool invincibility);

	void KeyState();
	void MouseState();

};