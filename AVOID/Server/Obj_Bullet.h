#pragma once

#include "Object.h"

constexpr int BULLET_RADIUS = 18;
constexpr int BULLET_SPEED = 15;

class Bullet : CObject {
public:
	Bullet() = default;
	// �Ѿ� ������ �߰�. �Ѿ� ����� �� ���� ������ �׾ �ȹٲ�.
	Bullet(int x, int y, int direction);

	Coord GetPosition() { return m_position; };
	int GetRadius() { return radius; };
	int GetBulletSpeed() { return bulletSpeed; };

	void SetBulletSpeed(int speed) { bulletSpeed = speed; };

	void Update(float fTimeElapsed);
private:
	Coord m_position;

	int m_iDirection;
	float TimeDelay;


	int radius = BULLET_RADIUS;
	int bulletSpeed;
};
