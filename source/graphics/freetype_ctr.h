#pragma once

// C++ STL
//#include <codecvt>
//#include <locale>
#include <map>
#include <string>

// 3DS Libraries
#include <3ds.h>
#include <citro3d.h>

// Freetype 2
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H

// Graphics Subsystem
#include "core.h"

namespace GFX {

class Font;

class Glyph {
public:
	u32 width;
	u32 height;
	C3D_Tex texture;
	u32 advanceX;
	u32 bitmapTop;
	u32 bitmapLeft;

	Glyph(FT_Face face, char32_t c);
	~Glyph();

	void render(int x, int y) const;
};

class Font {
public:
	FT_Face face;

	Font(const u8 *data, size_t size, int faceIndex = 0);
	//Font(std::string path, int faceIndex = 0);

	~Font();

	const Glyph &getGlyph(char32_t c);

	void drawText(const std::u32string &str, int x, int y, const GFX::Color &color);
private:
	std::map<char32_t, Glyph> glyphs;
};

}
