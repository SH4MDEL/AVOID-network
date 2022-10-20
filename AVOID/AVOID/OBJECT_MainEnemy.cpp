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
	//Siren.Destroy();
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

	double d = 0;					// 중심원과 총알 사이의 거리
	double radius = 200;

	for (p = this->head; p != nullptr; p = p->nextbullet) {
		d = sqrt((double)(pow((double)windowX / 2 - (double)p->bulletx, 2) + pow((double)windowY / 2 - (double)p->bullety, 2)));

		if (((radius - (double)p->radius) < d && d < (radius + (double)p->radius) && (p->GetCrashTime() >= 1.f)) ||			// 두점에서 만나는 경우
			((radius - p->radius) < d && p->GetCrashTime() >= 0.5f))														// 총알이 생성된지 1초가 지나고 원밖에 나간 경우
		{				
			DeleteBullet(p);
			break;						// break를 안걸 시 프로그램 죽음 p 를 삭제했을때 p를 delete 했기에 p= p->nextbullet 에서 오류 발생
		}
	}

}

void OBJECT_MainEnemy::DeleteBullet(OBJECT_Bullet* bullet)
{
	OBJECT_Bullet* removal;
	removal = bullet;

	if (bullet == this->head) {										// 삭제할 총알이 head 인 경우

		if (bullet->nextbullet == nullptr) {						// 삭제할 총알이 head 이면서 한개인 경우
			this->head = nullptr;
			delete removal;
			//printf("헤드노드 삭제1\n");
			return;
		}

		else {
			bullet->nextbullet->prebullet = nullptr;
			this->head = bullet->nextbullet;
			delete removal;
			//printf("헤드노드 삭제2\n");
			return;
		}
	}

	else {															// 삭제할 총알이 head가 아닌경우

		if (bullet->nextbullet == nullptr) {						// 삭제할 총알이 마지막 총알인 경우
			bullet->prebullet->nextbullet = nullptr;
			delete removal;
			//printf("테일노드 삭제\n");
			return;
		}

		else {														//삭제할 총알이 중간 총알인 경우
			bullet->prebullet->nextbullet = bullet->nextbullet;
			bullet->nextbullet->prebullet = bullet->prebullet;
			delete removal;
			//printf("중간노드 삭제\n");
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