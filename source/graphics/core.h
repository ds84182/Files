#pragma once

#include <3ds.h>
#include <citro3d.h>

namespace GFX {

extern C3D_MtxStack stack;

enum class Screen {
	TopLeft,
	TopRight,
	Bottom
};

// TODO: Make this into some class thing that can be moved around and stuff
extern enum class FragMode {
	Unset,
	Replace,
	ModulateTexture
} currentFragMode;

union Color {
	struct {
		u8 r, g, b, a;
	};
	u32 raw;

	Color() : r(0), g(0), b(0), a(255) {};
	Color(u8 r, u8 g, u8 b, u8 a = 255) : r(r), g(g), b(b), a(a) {};
	Color(u32 raw) : raw(raw) {};

	Color operator+(const Color &other) const {
		return Color(r+other.r, g+other.g, b+other.b, a+other.a);
	}

	Color operator-(const Color &other) const {
		return Color(r-other.r, g-other.g, b-other.b, a-other.a);
	}

	Color operator*(const float &other) const {
		return Color(r*other, g*other, b*other, a*other);
	}
};

static_assert(sizeof(Color) == 4, "Size of color union is not 4 bytes!");

class FrameBuffer;
extern FrameBuffer TopLeft;
extern FrameBuffer TopRight;
extern FrameBuffer Bottom;

extern int textureScaleUniform;
extern int textureOffsetUniform;

void Init();
void Deinit();

void FrameBegin();
void DrawOn(Screen screen);
void DrawOn(GFX::FrameBuffer *fb, C3D_Mtx *projection, bool notFlipped = false);
void FrameEnd();

void EnableTexturing();
void DisableTexturing();

inline void SetFragMode(FragMode mode) {
	if (mode != currentFragMode) {
		if (mode == FragMode::Replace) {
			// Configure the first fragment shading substage to just pass through the vertex color
			// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
			C3D_TexEnv* env = C3D_GetTexEnv(0);
			C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, 0, 0);
			C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
			C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
		} else if (mode == FragMode::ModulateTexture) {
			// Configure the first fragment shading substage to blend the texture color with
			// the vertex color
			// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
			C3D_TexEnv* env = C3D_GetTexEnv(0);
			C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
			C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
			C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

			/*
			const FragMode ModulateTexture {
				TEV::Src(TEV::Mode::Both, TEV::Source::Texture0, TEV::Source::PrimaryColor),
				TEV::Op(TEV::Mode::Both),
				TEV::Func(TEV::Mode::Both, TEV::Func::Modulate)
			}

			ModulateTexture.use();
			*/
		}
		currentFragMode = mode;
	}
}

inline C3D_Mtx *PushMatrix() {
	MtxStack_Push(&stack);
	return MtxStack_Cur(&stack);
}

inline void UpdateMatrix() {
	MtxStack_Update(&stack);
}

inline void PopMatrix() {
	MtxStack_Pop(&stack);
}

}
