#pragma once
#include "Object.h"

constexpr int BULLET_RADIUS = 18;
constexpr int BULLET_SPEED = 15;

class Bullet : CObject {

	Coord x, y;

	int m_iDirection;
	float TimeDelay;

	int radius = BULLET_RADIUS;
	int bulletSpeed = BULLET_SPEED;
};
