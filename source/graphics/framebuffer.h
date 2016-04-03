#pragma once

#include <3ds.h>
#include <citro3d.h>

#include "core.h"

namespace GFX {

class FrameBuffer {
public:
	C3D_RenderTarget *renderTarget = nullptr;
	int width;
	int height;

	FrameBuffer() {}

	FrameBuffer(int width, int height, int colorFormat, int depthFormat) {
		create(width, height, colorFormat, depthFormat);
	}

	~FrameBuffer() {
		destroy();
	}

	void create(int width, int height, int colorFormat, int depthFormat) {
		this->width = width;
		this->height = height;
		renderTarget = C3D_RenderTargetCreate(width, height, colorFormat, depthFormat);
	}

	void destroy() {
		if (renderTarget) {
			C3D_RenderTargetDelete(renderTarget);
			renderTarget = nullptr;
		}
	}

	void setClear(u32 clearBits, GFX::Color clearColor, u32 clearDepth = 0) {
		C3D_RenderTargetSetClear(renderTarget, clearBits, clearColor.raw, clearDepth);
	}

	void setOutput(gfxScreen_t screen, gfx3dSide_t side, u32 transferFlags) {
		C3D_RenderTargetSetOutput(renderTarget, screen, side, transferFlags);
	}

	void drawOn() {
		C3D_FrameDrawOn(renderTarget);
	}
};

}
