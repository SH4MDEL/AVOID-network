#pragma once

#include "Object.h"

constexpr int BULLET_RADIUS = 18;
constexpr int BULLET_SPEED = 15;

class Bullet : CObject {
public:
	Bullet() = default;
	// 총알 생성자 추가. 총알 사라질 때 까지 방향은 죽어도 안바뀜.
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
