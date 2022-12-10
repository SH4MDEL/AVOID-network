#pragma once
#include "stdafx.h"

class Object
{
public:
	Object() = default;
	Object(Coord position);
	~Object() = default;

	virtual void Update(float timeElapsed) = 0;

	Coord GetPosition() { return m_position; }

	void SetPosition(Coord position) { m_position = position; }

protected:
	Coord		m_position;
};

