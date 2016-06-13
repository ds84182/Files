#include "scissor.h"

#include <cstdio>

using namespace GFX;

Bounds GFX::scissorBox;
u32 GFX::scissorFBHeight;
bool GFX::scissorFlipped;

void GFX::Scissor::updateScissor() {
	if (scissorFlipped) {
		C3D_SetScissor(GPU_SCISSOR_NORMAL,
			scissorFBHeight-((u32)scissorBox.bottom),
			scissorBox.left,
			scissorFBHeight-((u32)scissorBox.top),
			scissorBox.right);
	} else {
		C3D_SetScissor(GPU_SCISSOR_NORMAL,
			scissorBox.left,
			scissorBox.top,
			scissorBox.right,
			scissorBox.bottom);
	}
}
