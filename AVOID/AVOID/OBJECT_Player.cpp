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
		this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player1.png");				// 기본
	}
	if (m_playerID == 1) {
		this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player2.png");				// 기본
	}
	if (m_playerID == 2) {
		this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player3.png");				// 기본
	}
#endif // USE_NETWORK
#ifndef USE_NETWORK
	this->Player[0].Load(L"Graphic\\OBJECT\\PLAYER\\Player1.png");				// 기본
#endif // !USE_NETWORK
	this->Player[1].Load(L"Graphic\\OBJECT\\PLAYER\\Hit_Player1.png");				// 피격 1
	this->Player[2].Load(L"Graphic\\OBJECT\\PLAYER\\Hit_Player2.png");				// 피격 2
	this->Ability.Load(L"Graphic\\OBJECT\\PLAYER\\Ability.png");					// 능력

	this->x = x / 2;											// 화면의 정 중앙이 시작점
	this->y = y / 2;
	SetCursorPos(windowX / 2 + 9, windowY / 2 + 30);			// 화면 중앙에 커서를 조정함
	ShowCursor(false);								// 마우스 커서를 감춤
	//ShowCursor(true);								// Music1이 끝나면 보여줘야함

}

void OBJECT_Player::Update(float fTimeElapsed)
{
	hp_restore += fTimeElapsed;
	if (hp_restore >= 1.3f) {					// 3초마다 hp 회복
		if (Player_hp >= (float)100) {			// 기존의 hp가 100보다 크거나 같으면 hp는 100, hp회복 카운트 0
			Player_hp = 100.f;
			hp_restore = 0;
		}

		else {									// 100보다 작으면 hp1 회복, hp 회복 카운트 0
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

	if (this->invincibility) {					// 피격 후 무적이 되면
		this->TimeDelay += fTimeElapsed;		
		if (this->TimeDelay >= 2.f) {			// 2초후 TimeDelay를 0으로 만들고 무적을 해제함
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

	if (!invincibility) {			// 기본 피격x 상태
		Player[0].Draw(*hDC, this->x - Player_radius, this->y - Player_radius, Player_radius * 2, Player_radius * 2);
	}
	else {				// 피격후 무적이 켜지면
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

	hBrush = CreateSolidBrush(RGB(0, 0, 105));			// 옅은 파란색
	oldBrush = (HBRUSH)SelectObject(*hDC, hBrush);

	Rectangle(*hDC, 0, windowY - 72, windowX*(Player_mp / (float)(2.5) / 100), windowY - 50);		// mp에 따라 mp바의 길이가 달라짐

	SelectObject(*hDC, oldBrush);
	DeleteObject(hBrush);

	if (this->Player_hp < 70.f) {

		hBrush = CreateSolidBrush(RGB(105, 0, 0));			// 옅은 붉은색
		oldBrush = (HBRUSH)SelectObject(*hDC, hBrush);

		Rectangle(*hDC, 0, windowY - 50, windowX*(Player_hp / 100), windowY);		// hp에 따라 hp바의 길이가 달라짐

		SelectObject(*hDC, oldBrush);
		DeleteObject(hBrush);
	}

	else {

		hBrush = CreateSolidBrush(RGB(0, 105, 0));			// 옅은 붉은색
		oldBrush = (HBRUSH)SelectObject(*hDC, hBrush);

		Rectangle(*hDC, 0, windowY - 50, windowX*(Player_hp / 100), windowY);		// hp에 따라 hp바의 길이가 달라짐

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

void OBJECT_Player::SetHp(float hp)						// 피격 판정 시 hp가 달도록 설정
{
	this->Player_hp -= hp;
}

void OBJECT_Player::SetHp_zero()
{
	this->Player_hp = 0;
}

void OBJECT_Player::SetAbility(int ability)				// 곡 선택화면에서 선택하도록 함
{
	this->Player_ability = ability;
}

void OBJECT_Player::MouseState()
{
	
	//double d = sqrt((double)(pow((double)windowX / 2 - (double)this->x, 2) + pow((double)windowY / 2 - (double)this->y, 2)));		// 플레이어 원과 플레이 원 중심의 거리
	double d = sqrt((double)(pow((double)windowX / 2 - (double)mouse_mx, 2) + pow((double)windowY / 2 - (double)mouse_my, 2)));		// 플레이어 원과 플레이 원 중심의 거리
	double r_ = 200 - this->Player_radius;
	
	int tempx = this->x;
	int tempy = this->y;

	if (r_ >= d) {							// 커서가 원 내부
		this->x = mouse_mx;
		this->y = mouse_my;
	}
	else {									// 커서가 원 외부
		SetCursorPos(tempx + 9, tempy + 30);	// Cursor는 window창의 상단바를 포함?하는 듯 함			아닐 경우 tempx,tempy가 맞음
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