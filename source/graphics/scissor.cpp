#include "scissor.h"

#include <cstdio>

using namespace GFX;

Bounds GFX::scissorBox;
u32 GFX::scissorFBHeight;

void GFX::Scissor::updateScissor() {
	C3D_SetScissor(GPU_SCISSOR_NORMAL, scissorFBHeight-((u32)scissorBox.bottom), scissorBox.left, scissorFBHeight-((u32)scissorBox.top), scissorBox.right);
}
