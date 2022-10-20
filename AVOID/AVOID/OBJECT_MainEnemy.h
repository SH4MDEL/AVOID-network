#pragma once
#include "Object.h"

class OBJECT_Bullet;
class CFramework;

class OBJECT_MainEnemy : public CObject
{
private:
	float x = 0;									// x�� ��ǥ�� y�� ��ǥ. ������ ������ location�� ���� ���� �޶�����.
	float y = 0;									// �� �� MainEnemy�� ��ǥ�� �޶��� ����?
	float xydata = 0;

	float beatCount = 0;						// �̹����� �����̴µ� ���Ǵ� ����.
	int imageCount = 0;							// ���� ���õ� �̹����� ��Ÿ��

	bool draw = true;							// true�� ��� MainEnemy�� �׸�

	float pi = 3.14159265358979;
	int direction = 0;							// Ingame���κ��� ��Ʈ�� ������ ����. 0�ϰ�� �ƹ��͵� ���� ����.
	int bulletNum = 0;

	int selectedMusic = 0;
	int enemyc = 0;

public:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	OBJECT_Bullet* head;						// head ����Ű�� ������

	OBJECT_MainEnemy();							// �׳� ������ ���
	OBJECT_MainEnemy(int location, int selectedMusic);				// ��ǥ�� �־� ������ ���
												// �������� ��Ÿ����. 1���� 12���� ���� �� ������, 
												// �ش� ���ڴ� �ð� �������� �� ������Ʈ�� ������ ��ġ�� �ǹ�
	~OBJECT_MainEnemy();						// �Ҹ���

	CImage Enemy[2];
	//CImage Siren;

	void OnCreate(int location);				// ������ ��ǥ�� ������ �ش�.
	void Update(float fTimeElapsed, int lotateSpeed, int note);
	void Render(HDC* hdc);
	//void firstRender(HDC* hdc);

	void BulletCrash();							// ���� �Ѿ� �浹üũ
	void DeleteBullet(OBJECT_Bullet* bullet);

	void SetDrawFalse();				
	void SetDrawTrue();


};
