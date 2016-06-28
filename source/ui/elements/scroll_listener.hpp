#pragma once

#include <memory>

#include <ui/element.hpp>

namespace UI {
namespace Elements {

struct ScrollListener {
	virtual void onScroll(int displacement) = 0;
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
	int lastVelocity = 0;
	int scrollVelocity = 0;
	bool hasMoved = false;

	void hookCallbacks();
	void handleMove(int x, int y, bool up);
};

}
}
