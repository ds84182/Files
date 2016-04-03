#pragma once

#include <3ds.h>
#include <citro3d.h>

#include "core.h"

namespace GFX {

class Rectangle {
public:
	float x, y, width, height;
	GFX::Color color;

	Rectangle(float x, float y, float width, float height, GFX::Color color);

	void render();
};

}
