#include "stdafx.h"
#include "OBJECT_MainEnemy.h"
#include "OBJECT_Bullet.h"
#include "Framework.h"

OBJECT_MainEnemy::OBJECT_MainEnemy()						// ������
{

}

OBJECT_MainEnemy::OBJECT_MainEnemy(int location, int selectedMusic)
{
	OnCreate(location);
	this->selectedMusic = selectedMusic;
}

OBJECT_MainEnemy::~OBJECT_MainEnemy()						// �Ҹ���
{
	Enemy[0].Destroy();
	Enemy[1].Destroy();
	for (auto bullet : m_bullets) delete bullet;
	m_bullets.clear();
}

void OBJECT_MainEnemy::OnCreate(int location)				// OnCreate�� �ش� ������Ʈ�� ������ �ҷ��ݴϴ�. 
{
	this->Enemy[0].Load(L"Graphic\\OBJECT\\ENEMY\\Enemy1.png");
	this->Enemy[1].Load(L"Graphic\\OBJECT\\ENEMY\\Enemy2.png");
	//this->Siren.Load(L"Graphic\\OBJECT\\ENEMY\\Siren.png");

	this->xydata = location * pi / 6;
	this->x = (windowX / 2) + 220 * cos(xydata); // �ʱ� 12���� Enemy���� ��ǥ
	this->y = (windowY / 2) + 220 * sin(xydata);
	//		  (windowX / 2) + 220 * cos(location * pi / 6) <- ȭ���� �߾� + ���� ������ * �ﰢ�Լ�. ���� ������ ���� �ʼ�!!

	this->draw = true;	// �׷��� ���°� �ȴ�.
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

	double d = 0;					// �߽ɿ��� �Ѿ� ������ �Ÿ�
	double radius = 200;
	for (auto bullet = m_bullets.begin(); bullet != m_bullets.end(); ++bullet) {
		d = sqrt((double)(pow((double)windowX / 2 - (double)(*bullet)->bulletx, 2) + pow((double)windowY / 2 - (double)(*bullet)->bullety, 2)));
		if (((radius - (double)(*bullet)->radius) < d && d < (radius + (double)(*bullet)->radius) && ((*bullet)->GetCrashTime() >= 1.f)) ||			// �������� ������ ���
			((radius - (*bullet)->radius) < d && (*bullet)->GetCrashTime() >= 0.5f))														// �Ѿ��� �������� 1�ʰ� ������ ���ۿ� ���� ���
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