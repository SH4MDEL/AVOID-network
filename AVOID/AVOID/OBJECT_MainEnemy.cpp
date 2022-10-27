#include "stdafx.h"
#include "OBJECT_MainEnemy.h"
#include "OBJECT_Bullet.h"
#include "Framework.h"

OBJECT_MainEnemy::OBJECT_MainEnemy()						// 생성자
{

}

OBJECT_MainEnemy::OBJECT_MainEnemy(int location, int selectedMusic)
{
	OnCreate(location);
	this->selectedMusic = selectedMusic;
}

OBJECT_MainEnemy::~OBJECT_MainEnemy()						// 소멸자
{
	Enemy[0].Destroy();
	Enemy[1].Destroy();
	for (auto bullet : m_bullets) delete bullet;
	m_bullets.clear();
}

void OBJECT_MainEnemy::OnCreate(int location)				// OnCreate는 해당 오브젝트의 생성시 불러줍니다. 
{
	this->Enemy[0].Load(L"Graphic\\OBJECT\\ENEMY\\Enemy1.png");
	this->Enemy[1].Load(L"Graphic\\OBJECT\\ENEMY\\Enemy2.png");
	//this->Siren.Load(L"Graphic\\OBJECT\\ENEMY\\Siren.png");

	this->xydata = location * pi / 6;
	this->x = (windowX / 2) + 220 * cos(xydata); // 초기 12개의 Enemy들의 좌표
	this->y = (windowY / 2) + 220 * sin(xydata);
	//		  (windowX / 2) + 220 * cos(location * pi / 6) <- 화면의 중앙 + 원의 반지름 * 삼각함수. 원의 반지름 수정 필수!!

	this->draw = true;	// 그려진 상태가 된다.
}

void OBJECT_MainEnemy::Update(float fTimeElapsed, int lotateSpeed, int note)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	beatCount += fTimeElapsed;
	float SpeedData = 0;

	if (beatCount >= (2.f)) {											
		beatCount = 0.f;
		if (imageCount == 1) {
			imageCount = 0;
		}
		else {
			imageCount++;
		}
	}
	SpeedData = (float)lotateSpeed;
	/*		bbkkbkk			*/	
	if (selectedMusic == 1) {
		this->xydata += SpeedData * pi * 1 / 170 * (fTimeElapsed * (680 / 60));
		this->x = (windowX / 2) + 220 * cos(this->xydata);		
		this->y = (windowY / 2) + 220 * sin(this->xydata);
	}
	/*		TrueBlue		*/
	else if (selectedMusic == 2) {
		this->xydata += SpeedData * pi * 1 / 170 * (fTimeElapsed * (656 / 60));
		this->x = (windowX / 2) + 220 * cos(this->xydata);		
		this->y = (windowY / 2) + 220 * sin(this->xydata);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (note) {
		m_bullets.push_back(new OBJECT_Bullet{(int)x, (int)y, note});
	}
	for (auto bullet : m_bullets) (*bullet).Update(fTimeElapsed);

	BulletCrash();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void OBJECT_MainEnemy::Render(HDC* hDC)
{
	Enemy[imageCount].Draw(*hDC, this->x - 20, this->y - 20, 40, 40);

	for (auto bullet : m_bullets) (*bullet).Render(hDC);
}


void OBJECT_MainEnemy::BulletCrash()
{
	//OBJECT_Bullet *p;

	double d = 0;					// 중심원과 총알 사이의 거리
	double radius = 200;
	for (auto bullet = m_bullets.begin(); bullet != m_bullets.end(); ++bullet) {
		d = sqrt((double)(pow((double)windowX / 2 - (double)(*bullet)->bulletx, 2) + pow((double)windowY / 2 - (double)(*bullet)->bullety, 2)));
		if (((radius - (double)(*bullet)->radius) < d && d < (radius + (double)(*bullet)->radius) && ((*bullet)->GetCrashTime() >= 1.f)) ||			// 두점에서 만나는 경우
			((radius - (*bullet)->radius) < d && (*bullet)->GetCrashTime() >= 0.5f))														// 총알이 생성된지 1초가 지나고 원밖에 나간 경우
		{
			delete *bullet;
			m_bullets.erase(bullet);
			break;					
		}
	}

}

void OBJECT_MainEnemy::SetDrawFalse()
{
	this->draw = false;
}
void OBJECT_MainEnemy::SetDrawTrue()
{
	this->draw = true;
}