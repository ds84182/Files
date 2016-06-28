#include "scroll_listener.hpp"

namespace UI {
namespace Elements {

void ScrollListenerElement::render(float timeDelta) {
	if (scrollVelocity != 0) {
		data->onScroll(scrollVelocity);
		scrollVelocity *= 0.985f;
	}
}

void ScrollListenerElement::hookCallbacks() {
	onTouchStart = [&](int x, int y) {
		touchX = x;
		touchY = y;
		hasMoved = false;
		lastVelocity = 0;
		data->forwardTouchStart(x, y);
		return true; // We used the event to do something, return true
	};

	onTouchMove = [&](int x, int y) {
		hasMoved = true;
		data->forwardTouchMove(x, y);
		handleMove(x, y, false);
		return true; // We used the event to do something, return true
	};

	onTouchEnd = [&](int x, int y) {
		data->forwardTouchEnd(x, y);
		if (hasMoved) // Only handle move if onTouchMove was fired
			handleMove(x, y, true);
		return true; // We used the event to do something, return true
	};

	onTap = [&](int x, int y) {
		data->forwardTap(x, y); // Forward the tap to the listener to handle
		return true;
	};
}

void ScrollListenerElement::handleMove(int x, int y, bool up) {
	// TODO: Horizontal
	int diff = y-touchY;
	data->onScroll(diff);
	if (up) {
		scrollVelocity = lastVelocity;
	}
	lastVelocity = diff;
	touchX = x;
	touchY = y;
}

}
}
