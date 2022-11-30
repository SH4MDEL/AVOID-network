#include "Obj_Bullet.h"

Bullet::Bullet(int x, int y, int direction)
{
	m_position.x = x, m_position.y = y;
	m_iDirection = direction;
	TimeDelay = 0.0f;
}

void Bullet::Update(float fTimeElapsed)
{
	TimeDelay += fTimeElapsed;
	if (TimeDelay >= 0.05f) {
		float theta = (((float)9 - (float)m_iDirection) * pi / (float)4);
		m_position.x += (bulletSpeed * cos(theta) * (TimeDelay * (float)12));
		m_position.y += (bulletSpeed * sin(theta) * (TimeDelay * (float)12));
		TimeDelay = 0.0f;
	}
}
