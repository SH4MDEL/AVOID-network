#pragma once
#include "Object.h"

class OBJECT_Bullet;
class CFramework;

class OBJECT_MainEnemy : public CObject
{
public:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	std::list<OBJECT_Bullet*> m_bullets;

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

	void SetDrawFalse();				
	void SetDrawTrue();
	void SetPos(int x, int y);

	// ���� ���� �߰� �Լ��Դϴ�.
	int GetServerX() { return m_x - windowX / 2; }
	int GetServerY() { return m_y - windowY / 2; }
	// Ŭ���̾�Ʈ�� ���� ��ǥ�踦 ���� ��ǥ��� �����մϴ�.
	void SetServerPos(float x, float y)
	{ m_x = x + windowX / 2, m_y = y + windowY / 2; }
	// ���۵� ���� ��ǥ�踦 Ŭ���̾�Ʈ ��ǥ��� �����մϴ�.

private:
	float m_x = 0;									// x�� ��ǥ�� y�� ��ǥ. ������ ������ location�� ���� ���� �޶�����.
	float m_y = 0;									// �� �� MainEnemy�� ��ǥ�� �޶��� ����?
	float xydata = 0;

	float beatCount = 0;						// �̹����� �����̴µ� ���Ǵ� ����.
	int imageCount = 0;							// ���� ���õ� �̹����� ��Ÿ��

	bool draw = true;							// true�� ��� MainEnemy�� �׸�

	float pi = 3.14159265358979;
	int direction = 0;							// Ingame���κ��� ��Ʈ�� ������ ����. 0�ϰ�� �ƹ��͵� ���� ����.
	int bulletNum = 0;

	int selectedMusic = 0;
	int enemyc = 0;

};
