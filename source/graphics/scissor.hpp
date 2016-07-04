#pragma once

#include <citro3d.h>

#include <utils/bounds.hpp>

namespace GFX {

extern Bounds scissorBox;
extern u32 scissorFBHeight;
extern bool scissorFlipped;

class Scissor {
public:
	Scissor(Bounds current) : previous(scissorBox), current(current) {
		scissorBox = current;
		scissorBox.clip(previous);

		updateScissor();
	}

	~Scissor() {
		exit();
	}

	void exit() {
		if (invalid) return;
		scissorBox = previous;
		updateScissor();
		invalid = true;
	}

private:
	Bounds previous;
	Bounds current;
	bool invalid = false;

	static void updateScissor();
};

}
