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
		this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player1.png");				// �⺻
	}
	if (m_playerID == 1) {
		this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player2.png");				// �⺻
	}
	if (m_playerID == 2) {
		this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player3.png");				// �⺻
	}
#endif // USE_NETWORK
#ifndef USE_NETWORK
	this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player1.png");				// �⺻
#endif // !USE_NETWORK
	this->Player[1].Load(L"Graphic\\OBJECT\\PLAYER\\Hit_Player1.png");				// �ǰ� 1
	this->Player[2].Load(L"Graphic\\OBJECT\\PLAYER\\Hit_Player2.png");				// �ǰ� 2
	this->Ability.Load(L"Graphic\\OBJECT\\PLAYER\\Ability.png");					// �ɷ�

	this->x = x / 2;											// ȭ���� �� �߾��� ������
	this->y = y / 2;
	SetCursorPos(windowX / 2 + 9, windowY / 2 + 30);			// ȭ�� �߾ӿ� Ŀ���� ������
	ShowCursor(false);								// ���콺 Ŀ���� ����
	//ShowCursor(true);								// Music1�� ������ ���������

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
			Player_hp += 1;
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
			Player_mp += 3;
			mp_restore = 0;
		}
	}

	if (this->invincibility) {					// �ǰ� �� ������ �Ǹ�
		this->TimeDelay += fTimeElapsed;		
		if (this->TimeDelay >= 2.f) {			// 2���� TimeDelay�� 0���� ����� ������ ������
			this->TimeDelay = 0;
			this->invincibility = false;
		}
	}

	MouseState();
	KeyState();
}

void OBJECT_Player::Render(HDC *hDC)
{
	if (AbilityCheck) {
		Ability.Draw(*hDC, this->x - Ability_radius, this->y - Ability_radius, Ability_radius * 2, Ability_radius * 2);
	}

	if (!invincibility) {			// �⺻ �ǰ�x ����
		Player[0].Draw(*hDC, this->x - Player_radius, this->y - Player_radius, Player_radius * 2, Player_radius * 2);
	}
	else {				// �ǰ��� ������ ������
		if (((this->TimeDelay >= 0.3f) && (this->TimeDelay <= 0.4f)) ||
			((this->TimeDelay >= 0.6f) && (this->TimeDelay <= 0.7f)) ||
			((this->TimeDelay >= 0.9f) && (this->TimeDelay <= 1.f)) ||
			((this->TimeDelay >= 1.2f) && (this->TimeDelay <= 1.3f)) ||
			((this->TimeDelay >= 1.5f) && (this->TimeDelay <= 1.6f))) {
			Player[1].Draw(*hDC, this->x - Player_radius, this->y - Player_radius, Player_radius * 2, Player_radius * 2);
		}
		else {
			Player[2].Draw(*hDC, this->x - Player_radius, this->y - Player_radius, Player_radius * 2, Player_radius * 2);
		}
	}

	hBrush = CreateSolidBrush(RGB(0, 0, 105));			// ���� �Ķ���
	oldBrush = (HBRUSH)SelectObject(*hDC, hBrush);

	Rectangle(*hDC, 0, windowY - 72, windowX*(Player_mp / (float)(2.5) / 100), windowY - 50);		// mp�� ���� mp���� ���̰� �޶���

	SelectObject(*hDC, oldBrush);
	DeleteObject(hBrush);

	if (this->Player_hp < 70.f) {

		hBrush = CreateSolidBrush(RGB(105, 0, 0));			// ���� ������
		oldBrush = (HBRUSH)SelectObject(*hDC, hBrush);

		Rectangle(*hDC, 0, windowY - 50, windowX*(Player_hp / 100), windowY);		// hp�� ���� hp���� ���̰� �޶���

		SelectObject(*hDC, oldBrush);
		DeleteObject(hBrush);
	}

	else {

		hBrush = CreateSolidBrush(RGB(0, 105, 0));			// ���� ������
		oldBrush = (HBRUSH)SelectObject(*hDC, hBrush);

		Rectangle(*hDC, 0, windowY - 50, windowX*(Player_hp / 100), windowY);		// hp�� ���� hp���� ���̰� �޶���

		SelectObject(*hDC, oldBrush);
		DeleteObject(hBrush);

	}
}

int OBJECT_Player::GetX()
{
	return this->x;
}

int OBJECT_Player::GetY()
{
	return this->y;
}

float OBJECT_Player::GetHp()
{
	return this->Player_hp;
}

bool OBJECT_Player::GetState()
{
	return this->invincibility;
}

int OBJECT_Player::GetRadius()
{
	return this->Player_radius;
}

bool OBJECT_Player::GetAState()
{
	return this->AbilityCheck;
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
	
	//double d = sqrt((double)(pow((double)windowX / 2 - (double)this->x, 2) + pow((double)windowY / 2 - (double)this->y, 2)));		// �÷��̾� ���� �÷��� �� �߽��� �Ÿ�
	double d = sqrt((double)(pow((double)windowX / 2 - (double)mouse_mx, 2) + pow((double)windowY / 2 - (double)mouse_my, 2)));		// �÷��̾� ���� �÷��� �� �߽��� �Ÿ�
	double r_ = 200 - this->Player_radius;
	
	int tempx = this->x;
	int tempy = this->y;

	if (r_ >= d) {							// Ŀ���� �� ����
		this->x = mouse_mx;
		this->y = mouse_my;
	}
	else {									// Ŀ���� �� �ܺ�
		SetCursorPos(tempx + 9, tempy + 30);	// Cursor�� windowâ�� ��ܹٸ� ����?�ϴ� �� ��			�ƴ� ��� tempx,tempy�� ����
		this->x = tempx;
		this->y = tempy;
	}
}

void OBJECT_Player::KeyState() {
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		if (Player_mp > 10.f) {
			Player_mp -= 1.f;
			AbilityCheck = true;
			if (Player_mp <= 10.f) {
				AbilityCheck = false;
			}
		}
	}
	else {
		AbilityCheck = false;
	}
}