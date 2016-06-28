#pragma once

#include <memory>

#include <graphics/fonts.hpp>

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

	virtual void render(float timeDelta) override {};

private:
	int touchX, touchY;
	bool hasMoved = false;

	void hookCallbacks() {
		onTouchStart = [&](int x, int y) {
			touchX = x;
			touchY = y;
			hasMoved = false;
			data->forwardTouchStart(x, y);
			return true; // We used the event to do something, return true
		};

		onTouchMove = [&](int x, int y) {
			hasMoved = true;
			data->forwardTouchMove(x, y);
			handleMove(x, y);
			return true; // We used the event to do something, return true
		};

		onTouchEnd = [&](int x, int y) {
			data->forwardTouchEnd(x, y);
			if (hasMoved) // Only handle move if onTouchMove was fired
				handleMove(x, y);
			return true; // We used the event to do something, return true
		};

		onTap = [&](int x, int y) {
			data->forwardTap(x, y); // Forward the tap to the listener to handle
			return true;
		};
	}

	void handleMove(int x, int y) {
		// TODO: Horizontal
		int diff = touchY-y;
		data->onScroll(-diff);
		touchX = x;
		touchY = y;
	}
};

}
}
