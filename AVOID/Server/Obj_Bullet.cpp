#include "Obj_Bullet.h"

Bullet::Bullet(int x, int y, int direction)
{
	m_position.x = x, m_position.y = y;
	m_iDirection = direction;
}
