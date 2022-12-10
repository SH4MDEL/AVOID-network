#include "Bullet.h"

Bullet::Bullet(Coord position, int direction) : Object(position), m_speed(BULLET_SPEED), m_direction(direction)
{
}

void Bullet::Update(float timeElapsed)
{
	float theta = (((float)9 - (float)m_direction) * PI / (float)4);
	m_position.x += (m_speed * cos(theta) * (timeElapsed * (float)12));
	m_position.y += (m_speed * sin(theta) * (timeElapsed * (float)12));
}
