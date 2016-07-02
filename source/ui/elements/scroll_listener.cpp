#include "scroll_listener.hpp"

#include <graphics/circle.hpp>
#include <graphics/scissor.hpp>

#define TICKS_PER_SEC (268123480)

namespace UI {
namespace Elements {

void ScrollListenerElement::render(float timeDelta) {
	GFX::Scissor scissor(bounds);

	float dist = scrollVelocity*timeDelta;
	if (std::abs(dist) > 0.01) {
		int recvFeedback = data->onScroll(dist);
		if (recvFeedback != 0) {
			/*if (std::abs(scrollFeedback) < std::abs(recvFeedback)) {
				scrollFeedback = recvFeedback;
			}*/
			if (scrollVelocity > 0) {
				eeTop.onAbsorb(-scrollVelocity);
			} else {
				eeBottom.onAbsorb(scrollVelocity);
			}
			scrollVelocity = 0;
		}
		scrollVelocity *= 0.895f;
	}

	if (!eeTop.isFinished()) {
		eeTop.setSize(bounds.width(), bounds.height());
		eeTop.draw();
	}

	if (!eeBottom.isFinished()) {
		eeBottom.setSize(bounds.width(), bounds.height());
		eeBottom.draw(-bounds.width(), bounds.height(), 3.14, -bounds.width(), 0);
	}
}

void ScrollListenerElement::hookCallbacks() {
	onTouchStart = [&](int x, int y) {
		touchX = x;
		touchY = y;
		hasMoved = false;
		lastTick = 0;
		lastDiff = 0;
		lastFeedback = 0;
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
	u64 timeTick = svcGetSystemTick();

	if (!up) {
		int diff = y-touchY;
		int recvFeedback = data->onScroll(diff);
		if (recvFeedback != 0) {
			if (diff > 0) {
				eeTop.onPull(((float)diff)/((float)bounds.height()), ((float)x)/((float)bounds.width()));
				if (!eeBottom.isFinished()) {
					eeBottom.onRelease();
				}
			} else if (diff < 0) {
				eeBottom.onPull(-((float)diff)/((float)bounds.height()), 1.0f-((float)x)/((float)bounds.width()));
				if (!eeTop.isFinished()) {
					eeTop.onRelease();
				}
			}
		}
		lastFeedback = recvFeedback;
		lastDiff = diff;
		lastTick = timeTick;
	} else {
		eeTop.onRelease();
		eeBottom.onRelease();
		if (lastFeedback == 0) {
			float timeDiff = ((float)(timeTick-lastTick))/TICKS_PER_SEC;
			scrollVelocity = lastDiff/timeDiff;
		} else {
			scrollVelocity = 0;
		}
	}

	touchX = x;
	touchY = y;
}

}
}
