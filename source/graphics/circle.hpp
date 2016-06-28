#pragma once

#include <3ds.h>
#include <citro3d.h>

#include "core.hpp"

namespace GFX {

class Circle {
public:
	float x, y, radius;
	GFX::Color color;

	Circle(float x, float y, float radius, GFX::Color color);

	void render();
};

}
