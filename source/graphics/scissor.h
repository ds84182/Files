#pragma once

#include <citro3d.h>

#include "utils/bounds.h"

namespace GFX {

extern Bounds scissorBox;
extern u32 scissorFBHeight;

class Scissor {
public:
	Scissor(Bounds current) : previous(scissorBox), current(current) {
		scissorBox = current;
		scissorBox.clip(previous);

		//C3D_SetScissor(GPU_SCISSOR_NORMAL, scissorBox.top, scissorBox.right, scissorBox.bottom, scissorBox.left);
		updateScissor();
		//C3D_SetScissor(GPU_SCISSOR_NORMAL, scissorBox.bottom, scissorBox.right, scissorBox.top, scissorBox.left);
	}

	~Scissor() {
		scissorBox = previous;
		//C3D_SetScissor(GPU_SCISSOR_NORMAL, scissorBox.top, scissorBox.right, scissorBox.bottom, scissorBox.left);
		updateScissor();
		//C3D_SetScissor(GPU_SCISSOR_NORMAL, scissorBox.bottom, scissorBox.right, scissorBox.top, scissorBox.left);
	}

private:
	Bounds previous;
	Bounds current;
	
	static void updateScissor();
};

}
