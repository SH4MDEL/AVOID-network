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
	//Siren.Destroy();
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
	this->head = nullptr;
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

	OBJECT_Bullet* Node;
	OBJECT_Bullet* temp;

	if (note != 0) {
		Node = new OBJECT_Bullet(this->x, this->y, note);
		Node->nextbullet = nullptr;
		Node->prebullet = nullptr;

		if (this->head == nullptr) {
			this->head = Node;
		}

		else {
			temp = this->head;
			while (temp->nextbullet != nullptr) {
				temp = temp->nextbullet;
			}
			temp->nextbullet = Node;
			Node->prebullet = temp;
		}
	}

	for (Node = this->head; Node != nullptr; Node = Node->nextbullet) {
		Node->Update(fTimeElapsed);
	}

	BulletCrash();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void OBJECT_MainEnemy::Render(HDC* hDC)
{
	Enemy[imageCount].Draw(*hDC, this->x - 20, this->y - 20, 40, 40);

	OBJECT_Bullet* p;
	for (p = this->head; p != nullptr; p = p->nextbullet) {
		p->Render(hDC);
	}
}


void OBJECT_MainEnemy::BulletCrash()
{
	OBJECT_Bullet *p;

	double d = 0;					// �߽ɿ��� �Ѿ� ������ �Ÿ�
	double radius = 200;

	for (p = this->head; p != nullptr; p = p->nextbullet) {
		d = sqrt((double)(pow((double)windowX / 2 - (double)p->bulletx, 2) + pow((double)windowY / 2 - (double)p->bullety, 2)));

		if (((radius - (double)p->radius) < d && d < (radius + (double)p->radius) && (p->GetCrashTime() >= 1.f)) ||			// �������� ������ ���
			((radius - p->radius) < d && p->GetCrashTime() >= 0.5f))														// �Ѿ��� �������� 1�ʰ� ������ ���ۿ� ���� ���
		{				
			DeleteBullet(p);
			break;						// break�� �Ȱ� �� ���α׷� ���� p �� ���������� p�� delete �߱⿡ p= p->nextbullet ���� ���� �߻�
		}
	}

}

void OBJECT_MainEnemy::DeleteBullet(OBJECT_Bullet* bullet)
{
	OBJECT_Bullet* removal;
	removal = bullet;

	if (bullet == this->head) {										// ������ �Ѿ��� head �� ���

		if (bullet->nextbullet == nullptr) {						// ������ �Ѿ��� head �̸鼭 �Ѱ��� ���
			this->head = nullptr;
			delete removal;
			//printf("����� ����1\n");
			return;
		}

		else {
			bullet->nextbullet->prebullet = nullptr;
			this->head = bullet->nextbullet;
			delete removal;
			//printf("����� ����2\n");
			return;
		}
	}

	else {															// ������ �Ѿ��� head�� �ƴѰ��

		if (bullet->nextbullet == nullptr) {						// ������ �Ѿ��� ������ �Ѿ��� ���
			bullet->prebullet->nextbullet = nullptr;
			delete removal;
			//printf("���ϳ�� ����\n");
			return;
		}

		else {														//������ �Ѿ��� �߰� �Ѿ��� ���
			bullet->prebullet->nextbullet = bullet->nextbullet;
			bullet->nextbullet->prebullet = bullet->prebullet;
			delete removal;
			//printf("�߰���� ����\n");
			return;
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