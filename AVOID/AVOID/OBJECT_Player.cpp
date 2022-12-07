#include "stdafx.h"
#include "OBJECT_Player.h"
#include "Framework.h"

OBJECT_Player::OBJECT_Player()
{

}

OBJECT_Player::OBJECT_Player(int x, int y)
{
	OnCreate(x, y);
}

OBJECT_Player::OBJECT_Player(int x, int y, int playerID) : m_playerID(playerID)
{
	OnCreate(x, y);
}

OBJECT_Player::~OBJECT_Player()
{
	Player[0].Destroy();
	Player[1].Destroy();
	Player[2].Destroy();
	Ability.Destroy();
}

void OBJECT_Player::OnCreate(int x, int y)
{
#ifdef USE_NETWORK
	if (m_playerID == 0) {
		this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player1.png");
	}
	if (m_playerID == 1) {
		this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player2.png");
	}
	if (m_playerID == 2) {
		this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player3.png");
	}
#endif // USE_NETWORK
#ifndef USE_NETWORK
	this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player1.png");				// �⺻
#endif // !USE_NETWORK
	Player[1].Load(L"Graphic\\OBJECT\\PLAYER\\Hit_Player1.png");				// �ǰ� 1
	Player[2].Load(L"Graphic\\OBJECT\\PLAYER\\Hit_Player2.png");				// �ǰ� 2
	Ability.Load(L"Graphic\\OBJECT\\PLAYER\\Ability.png");					// �ɷ�

	m_x = x / 2;											// ȭ���� �� �߾��� ������
	m_y = y / 2;
	SetCursorPos(windowX / 2 + 9, windowY / 2 + 30);			// ȭ�� �߾ӿ� Ŀ���� ������
	ShowCursor(false);								// ���콺 Ŀ���� ����

}

void OBJECT_Player::Update(float fTimeElapsed)
{
	hp_restore += fTimeElapsed;
	if (hp_restore >= 1.3f) {					// 3�ʸ��� hp ȸ��
		if (Player_hp >= (float)100) {			// ������ hp�� 100���� ũ�ų� ������ hp�� 100, hpȸ�� ī��Ʈ 0
			Player_hp = 100.f;
			hp_restore = 0;
		}

		else {									// 100���� ������ hp1 ȸ��, hp ȸ�� ī��Ʈ 0
			Player_hp += 2;
			hp_restore = 0;
		}
	}

	mp_restore += fTimeElapsed;
	if (mp_restore >= 1.f) {
		if (Player_mp >= (float)250) {
			Player_mp = 250.f;
			mp_restore = 0;
		}

		else {
			Player_mp += 4;
			mp_restore = 0;
		}
	}

	if (invincibility) {					// �ǰ� �� ������ �Ǹ�
		TimeDelay += fTimeElapsed;		
		if (TimeDelay >= 2.f) {			// 2���� TimeDelay�� 0���� ����� ������ ������
			TimeDelay = 0;
			invincibility = false;
		}
	}

	MouseState();

	KeyState();
}

void OBJECT_Player::Render(HDC *hDC)
{
	if (m_abilityCheck) {
		Ability.Draw(*hDC, m_x - Ability_radius, m_y - Ability_radius, Ability_radius * 2, Ability_radius * 2);
	}

	if (!invincibility) {			// �⺻ �ǰ�x ����
		Player[0].Draw(*hDC, m_x - Player_radius, m_y - Player_radius, Player_radius * 2, Player_radius * 2);
	}
	else {				// �ǰ��� ������ ������
		if (((this->TimeDelay >= 0.3f) && (this->TimeDelay <= 0.4f)) ||
			((this->TimeDelay >= 0.6f) && (this->TimeDelay <= 0.7f)) ||
			((this->TimeDelay >= 0.9f) && (this->TimeDelay <= 1.f)) ||
			((this->TimeDelay >= 1.2f) && (this->TimeDelay <= 1.3f)) ||
			((this->TimeDelay >= 1.5f) && (this->TimeDelay <= 1.6f))) {
			Player[1].Draw(*hDC, m_x - Player_radius, m_y - Player_radius, Player_radius * 2, Player_radius * 2);
		}
		else {
			Player[2].Draw(*hDC, m_x - Player_radius, m_y - Player_radius, Player_radius * 2, Player_radius * 2);
		}
	}
}

int OBJECT_Player::GetX()
{
	return m_x;
}

int OBJECT_Player::GetY()
{
	return m_y;
}

float OBJECT_Player::GetHp()
{
	return Player_hp;
}

float OBJECT_Player::GetMp()
{
	return Player_mp;
}

bool OBJECT_Player::GetState()
{
	return invincibility;
}

int OBJECT_Player::GetRadius()
{
	return Player_radius;
}

bool OBJECT_Player::GetAbilityState()
{
	return m_spaceKeyDown;
}

int OBJECT_Player::GetID()
{
	return m_playerID;
}

void OBJECT_Player::SetState(bool invincibility)
{
	this->invincibility = invincibility;
}

void OBJECT_Player::SetHp(float hp)						// �ǰ� ���� �� hp�� �޵��� ����
{
	this->Player_hp -= hp;
}

void OBJECT_Player::SetHp_zero()
{
	this->Player_hp = 0;
}

void OBJECT_Player::SetAbility(int ability)				// �� ����ȭ�鿡�� �����ϵ��� ��
{
	this->Player_ability = ability;
}

void OBJECT_Player::MouseState()
{
	double d = sqrt((double)(pow((double)windowX / 2 - (double)mouse_mx, 2) + pow((double)windowY / 2 - (double)mouse_my, 2)));		
	// �÷��̾� ���� �÷��� �� �߽��� �Ÿ�
	double r_ = 200 - Player_radius;

	if (r_ >= d) {							// Ŀ���� �� ����
		m_x = mouse_mx;
		m_y = mouse_my;
	}
	else {									// Ŀ���� �� �ܺ�
		m_x = (mouse_mx - windowX / 2) / d * r_ + windowX / 2;
		m_y = (mouse_my - windowY / 2) / d * r_ + windowY / 2;
		SetCursorPos(m_x + 9, m_y + 30);
	}
}

void OBJECT_Player::KeyState() {
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		if (Player_mp > 10.f) {
			Player_mp -= 1.f;
			m_spaceKeyDown = true;
			if (Player_mp <= 10.f) {
				m_spaceKeyDown = false;
			}
		}
	}
	else {
		m_spaceKeyDown = false;
	}
}