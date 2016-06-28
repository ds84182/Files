#pragma once

#include <3ds.h>
#include <citro3d.h>

#include "core.hpp"
#include "texture.hpp"

namespace GFX {

class Rectangle {
public:
	float x, y, width, height;
	GFX::Color color;

	Rectangle(float x, float y, float width, float height, GFX::Color color);

	void render(Texture *texture = nullptr, int textureWidth = 0, int textureHeight = 0,
		int textureX = 0, int textureY = 0);
};

}
