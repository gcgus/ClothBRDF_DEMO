#pragma once

#include "framework.h"

class light
{
public:
	Vector3 pos;
	Vector3 color;

	light();
	light(Vector3 pos, Vector3 color);

	void set(Vector3 pos, Vector3 color);

	void light::renderInMenu();
};

