#pragma once

// 3DS Libraries
#include <3ds.h>
#include <citro3d.h>

// Graphics Subsystem
#include "core.h"

namespace GFX {

// TODO: Move to core
static u32 np2(u32 v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

class Texture {
public:
	bool owns;
	C3D_Tex *tex;
	u16 width, height;
	u16 p2Width, p2Height;

	Texture() {
		owns = false;
		tex = nullptr;
	}

	Texture(u32 width, u32 height, GPU_TEXCOLOR format, bool vram = false) {
		create(width, height, format, vram);
	}

	Texture(C3D_Tex *tex) {
		this->tex = tex;
		owns = false;
		width = p2Width = tex->width;
		height = p2Height = tex->height;
	}

	~Texture() {
		destroy();
	}

	static Texture from(C3D_Tex *tex) {
		return Texture(tex);
	}

	void create(u32 width, u32 height, GPU_TEXCOLOR format, bool vram = false) {
		tex = new C3D_Tex();
		memset(tex, 0, sizeof(C3D_Tex));
		owns = true;

		p2Width = np2(width);
		p2Height = np2(height);
		this->width = width;
		this->height = height;

		if (vram)
			C3D_TexInitVRAM(tex, this->p2Width, this->p2Height, format);
		else
			C3D_TexInit(tex, this->p2Width, this->p2Height, format);
	}

	void destroy() {
		if (owns && tex) {
			C3D_TexDelete(tex);
			delete tex;
			tex = nullptr;
		}
	}

	void upload(const void *data) {
		C3D_TexUpload(tex, data);
	}

	void bind(int unit) {
		C3D_TexBind(unit, tex);
	}

	void copyFrom(Texture *other) {
		//u32 flags = 4;

		GSPGPU_FlushDataCache(other->tex->data, other->tex->size);

		/*GX_TextureCopy(reinterpret_cast<u32*>(other->tex->data),
			other->p2Width | (other->p2Width << 16),
			reinterpret_cast<u32*>(tex->data),
			p2Width | (p2Width << 16),
			512 * 4,
			flags);*/
		GX_DisplayTransfer(
			reinterpret_cast<u32*>(other->tex->data),
			GX_BUFFER_DIM(other->p2Width, other->p2Height),
			reinterpret_cast<u32*>(tex->data),
			GX_BUFFER_DIM(p2Width, p2Height),
			GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8)
		);
		gspWaitForPPF();
	}
};

}
