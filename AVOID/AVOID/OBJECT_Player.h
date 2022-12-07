#pragma once
#include "Object.h"

extern int mouse_mx;
extern int mouse_my;

class OBJECT_Player : public CObject
{
public:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	OBJECT_Player();
	OBJECT_Player(int x, int y);						//
	OBJECT_Player(int x, int y, int playerID);
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
	float GetMp();
	bool GetState();				// �������� ���¸� �޴� �Լ�
	int GetRadius();				// ������ �޴� �Լ�
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

	// ���� ���� �߰� �Լ��Դϴ�.
	int GetServerX() { return m_x - windowX / 2; }	
	int GetServerY() { return m_y - windowY / 2; }
	// Ŭ���̾�Ʈ�� ���� ��ǥ�踦 ���� ��ǥ��� �����մϴ�.
	void SetServerPos(float x, float y) 
	{ m_x = x + windowX / 2, m_y = y + windowY / 2; }
	// ���۵� ���� ��ǥ�踦 Ŭ���̾�Ʈ ��ǥ��� �����մϴ�.
#ifdef USE_NETWORK
	void SetPlayerID(int playerID) { m_playerID = playerID; }
#endif
private:
	float m_x = 0;											// x��ǥ
	float m_y = 0;											// y��ǥ
	float Player_hp = 30;									// �÷��̾��� hp
	float hp_restore = 0;
	float Player_mp = 250;
	float mp_restore = 0;

	float TimeDelay = 0;

	bool invincibility = false;								// �ǰݽ� ���� Ȯ�ο� ����
	int Player_radius = 10;									// �÷��̾� ���� ������			 �ٲ� �� Bullet�� Ư����� �浹üũ�� �������� �ٲ������
	int Player_ability = 1;									// Ư���ɷ�	
	bool m_abilityCheck;								// �ɷ��� �ߵ������� Ȯ���ϴ� ����
	bool m_spaceKeyDown;
	bool CheckKey = false;

	// ���� ���� �߰� �����Դϴ�.
	int m_playerID;
};