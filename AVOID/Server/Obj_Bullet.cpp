#include "Obj_Bullet.h"

Bullet::Bullet(int x, int y, int direction)
{
	m_position.x = x, m_position.y = y;
	m_iDirection = direction;
	TimeDelay = 0.0f;
	bulletSpeed = BULLET_SPEED;
}

void Bullet::Update(float fTimeElapsed)
{
	float theta = (((float)9 - (float)m_iDirection) * pi / (float)4);
	m_position.x += (bulletSpeed * cos(theta) * (fTimeElapsed * (float)12));
	m_position.y += (bulletSpeed * sin(theta) * (fTimeElapsed * (float)12));
}
