#pragma once
#include "Object.h"

extern int mouse_mx;
extern int mouse_my;

//int finalhp = 0;

class OBJECT_Player : public CObject
{
private:
	float x = 0;											// x��ǥ
	float y = 0;											// y��ǥ
	float Player_hp = 30;									// �÷��̾��� hp
	float hp_restore = 0;
	float Player_mp = 250;
	float mp_restore = 0;

	float TimeDelay = 0;

	bool invincibility = false;								// �ǰݽ� ���� Ȯ�ο� ����
	int Player_radius = 10;									// �÷��̾� ���� ������			 �ٲ� �� Bullet�� Ư����� �浹üũ�� �������� �ٲ������
	int Player_ability = 1;									// Ư���ɷ�	
	bool AbilityCheck = true;								// �ɷ��� �ߵ������� Ȯ���ϴ� ����
	bool CheckKey = false;

	HBRUSH hBrush, oldBrush;

public:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	OBJECT_Player();
	OBJECT_Player(int x, int y);						//
	~OBJECT_Player();

	CImage Player[3];
	CImage Ability;

	float hitHp = 10;									// �ѹ��� �ǰ� �� 10�� �޵��� ������
	int Ability_radius = 85;

	void OnCreate(int x, int y);
	void Update(float fTimeElapsed);
	void Render(HDC* hDC);

	int GetX();
	int GetY();
	float GetHp();
	bool GetState();				// �������� ���¸� �޴� �Լ�
	int GetRadius();				// ������ �޴� �Լ�
	bool GetAState();

	void SetHp(float hp);
	void SetHp_zero();
	void SetAbility(int ability);
	void SetState(bool invincibility);

	void KeyState();
	void MouseState();

};