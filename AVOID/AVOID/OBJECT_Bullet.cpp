#include "stdafx.h"
#include "OBJECT_Bullet.h"


OBJECT_Bullet::OBJECT_Bullet()
{
}

OBJECT_Bullet::OBJECT_Bullet(int x, int y, int direction)
{
	OnCreate(x, y, direction);
}


OBJECT_Bullet::~OBJECT_Bullet()
{
	bullet.Destroy();
}

void OBJECT_Bullet::OnCreate()
{
	this->bullet.Load(L"Graphic\\OBJECT\\BULLET\\Bullet.png");
	this->SetDrawTrue();
}

void OBJECT_Bullet::OnCreate(int x, int y, int direction)
{
	this->bullet.Load(L"Graphic\\OBJECT\\BULLET\\Bullet.png");
	this->bulletx = x;
	this->bullety = y;

	this->direction = direction;
	//printf("direction = %d\n", direction);
	this->SetDrawTrue();

}

void OBJECT_Bullet::Update(float fTimeElapsed)
{
	TimeDelay += fTimeElapsed;
	CrashTime += fTimeElapsed;
	if (TimeDelay >= 0.05f) {						// 0.05초마다..
		xydata = (((float)9 - ((float)direction)) * pi / (float)4);
		this->bulletx += (BulletSpeed * cos(xydata) * (TimeDelay * (float)20));		// 앞의 float 이 속도 제어		 기본값 15
		this->bullety += (BulletSpeed * sin(xydata) * (TimeDelay * (float)20));
		TimeDelay = 0;
	}
}

void OBJECT_Bullet::Render(HDC* hDC)
{
	bullet.Draw(*hDC, this->bulletx - this->radius, this->bullety - this->radius, this->radius * 2, this->radius * 2);
}

void OBJECT_Bullet::SetDrawFalse()
{
	this->draw = false;
}

void OBJECT_Bullet::SetDrawTrue()
{
	this->draw = true;
}

int OBJECT_Bullet::GetX()
{
	return this->bulletx;
}

int OBJECT_Bullet::GetY()
{
	return this->bullety;
}

float OBJECT_Bullet::GetCrashTime()
{
	return this->CrashTime;
}

void OBJECT_Bullet::SetPos(int x, int y)
{
	bulletx = x, bullety = y;
}

void OBJECT_Bullet::Setx(int x)
{
	this->bulletx = x;
}

void OBJECT_Bullet::Sety(int y)
{
	this->bullety = y;
}