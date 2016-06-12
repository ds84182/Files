#pragma once

// 3DS Libraries
#include <3ds.h>
#include <citro3d.h>

// Graphics Subsystem
#include "core.h"

namespace GFX {

class Texture : public C3D_Tex {
public:
	Texture(u32 width, u32 height, GPU_TEXCOLOR format, bool vram = false) {
		this->data = nullptr;
		if (vram)
			C3D_TexInitVRAM(this, width, height, format);
		else
			C3D_TexInit(this, width, height, format);
	}

	~Texture() {
		C3D_TexDelete(this);
	}

	static Texture *from(C3D_Tex *tex) {
		return reinterpret_cast<Texture*>(tex);
	}

	void upload(const void *data) {
		C3D_TexUpload(this, data);
	}

	void bind(int unit) {
		C3D_TexBind(unit, this);
	}
};

}
