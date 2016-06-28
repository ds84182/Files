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
#include "core.hpp"

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
	float baseline;

	Glyph(FT_Face face, char32_t c, float baseline);
	~Glyph();

	void render(int x, int y) const;
};

class Font {
public:
	FT_Face face = nullptr;

	Font(const u8 *data, size_t size, int fontSize);
	Font(const Font &font) = delete;
	Font(Font &font) = delete;
	Font() {};

	~Font();

	void init(const u8 *data, size_t size, int fontSize);

	const Glyph &getGlyph(char32_t c);

	void drawText(const std::u32string &str, int x, int y, const GFX::Color &color);
	int width(const std::u32string &str);
	int height() {return calcHeight;}
private:
	std::map<char32_t, Glyph> glyphs;
	int calcHeight;
	float baseline;
};

}
