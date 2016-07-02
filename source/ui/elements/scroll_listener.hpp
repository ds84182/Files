#pragma once

#include <3ds.h>

#include <memory>

#include <ui/element.hpp>

#include "edge_effect.hpp"

namespace UI {
namespace Elements {

struct ScrollListener {
	// Returns feedback (how much the displacement didn't apply because of boundaries)
	virtual int onScroll(int displacement) = 0;
	virtual void forwardTouchStart(int x, int y) {};
	virtual void forwardTouchMove(int x, int y) {};
	virtual void forwardTouchEnd(int x, int y) {};
	virtual void forwardTap(int x, int y) {};
};

class ScrollListenerElement : public Element<ScrollListener*> {
public:
	ScrollListenerElement() {
		hookCallbacks();
	}
	ScrollListenerElement(ScrollListener *data) : Element<ScrollListener*>(data) {
		hookCallbacks();
	}
	virtual ~ScrollListenerElement() = default;

	virtual void render(float timeDelta) override;

private:
	int touchX, touchY;
	int lastDiff = 0;
	u64 lastTick = 0;
	int lastFeedback = 0;
	float scrollVelocity = 0;
	bool hasMoved = false;

	EdgeEffect eeTop;
	EdgeEffect eeBottom;

	void hookCallbacks();
	void handleMove(int x, int y, bool up);
};

}
}
