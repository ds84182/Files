#pragma once

#include <deque>
#include <functional>
#include <limits>
#include <memory>
#include <vector>

#include "ui/layer.h"
#include "ui/layout.h"
#include "ui/manager.h"

#include "ui/elements/scroll_listener.h"

namespace UI {
namespace Layouts {

using UI::Elements::ScrollListenerElement;
using UI::Elements::ScrollListener;

template <typename Element, bool horizontal = false, typename Data = typename Element::Data>
class RecyclerLayout : public Layout, public ScrollListener {
	static_assert(!horizontal, "Horizontal RecyclerLayout currently not supported!");

	std::deque<std::shared_ptr<Element>> queue;
	Layer elementLayer;
	Layer scrollLayer;
	std::shared_ptr<ScrollListenerElement> scrollListener;
	std::weak_ptr<ElementBase> focused;
	int top = 0; // the data index of the element at the top of the queue

public:
	std::vector<Data> data;
	// Gets the size of an element
	//std::function<int(const Data&, int)> getElementSize;
	int elementSize; // < This is used if getElementSize is null
	std::function<void(Data&, int)> onSelected;

	RecyclerLayout() {
		scrollListener = std::make_shared<ScrollListenerElement>(this);
		scrollLayer.add(scrollListener);
	}

	~RecyclerLayout() {
		scrollLayer.clear();
		elementLayer.clear();
	}

	virtual void onScroll(int amount) override {
		for (auto &element : queue) {
			element->bounds.move(0, amount);
		}

		update();
	}

	virtual void forwardTouchStart(int x, int y) override {
		focused = elementLayer.findAt(x, y);

		auto element = focused.lock();
		if (element && element->onTouchStart) {
			element->onTouchStart(x, y);
		}
	}
	virtual void forwardTouchMove(int x, int y) override {
		auto element = focused.lock();
		if (element && element->onTouchMove) {
			element->onTouchMove(x, y);
		}
	}
	virtual void forwardTouchEnd(int x, int y) override {
		auto element = focused.lock();
		if (element && element->onTouchEnd) {
			element->onTouchEnd(x, y);
		}
	}
	virtual void forwardTap(int x, int y) override {
		if (auto element = focused.lock()) {
			if (element->onTap) {
				element->onTap(x, y);
			}

			if (onSelected) {
				int index = top;
				for (auto &e : queue) {
					if (element == e) break;
					index++;
				}

				onSelected(data.at(index), index);
			}
		}
	}

	void clear() {
		queue.clear();
		elementLayer.clear();
		data.clear();
		top = 0;
	}

	/*
	Update:
		For each element in the queue starting from the bottom: (NO ITERATORS)
			If the element's bounds are outside of the layout bounds:
				Remove element from queue (and from UI)
			Else:
				Break (No elements after this should be outside)

		For each element in the queue starting from the top: (NO ITERATORS)
			Do the same as the previous for each loop. (Except top++ after remove)

		Bounds fillBounds;
		For each element in the queue:
			Extend the fillBounds by element bounds.

		While fillBounds.bottom < bounds.bottom and data is left at the bottom
			Compute element size (either using constant or getElementSize).
			Note: LRTB (Left right top bottom)
			Bounds elementBounds(bounds.left, bounds.right, currentBottom.bounds.bottom, currentBottom.bounds.bottom+elementSize)
			Create element and add to queue (and to UI)
			fillBounds.extend(elementBounds);

		While fillBounds.top > bounds.top and data is left at the top
			Do the same as previous loop. (Except top-- after addition)
	*/
	virtual void update() override {
		// Update bounds for layers and the scroll listener
		elementLayer.bounds = bounds;
		scrollLayer.bounds = bounds;
		scrollListener->bounds = bounds;

		// Remove outside elements at the bottom
		while (!queue.empty()) {
			std::shared_ptr<Element> &element = queue.back();
			if (!element->bounds.contains(bounds)) {
				elementLayer.remove(element);
				queue.pop_back();
			} else {
				// We expect every element above this to be inside
				break;
			}
		}

		// Remove outside elements at the top
		while (!queue.empty()) {
			std::shared_ptr<Element> &element = queue.front();
			if (!element->bounds.contains(bounds)) {
				elementLayer.remove(element);
				queue.pop_front();
				top++;
			} else {
				// We expect every element above this to be inside
				break;
			}
		}

		// Attempt to fit new elements into the view
		Bounds fillBounds(LRTB, std::numeric_limits<int>::max(), std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), std::numeric_limits<int>::min());
		for (std::shared_ptr<Element> &element : queue) {
			fillBounds.extend(element->bounds);
		}

		if (top == 0 && fillBounds.top > bounds.top) {
			// move all elements up
			int diff = fillBounds.top-bounds.top;
			for (std::shared_ptr<Element> &element : queue) {
				element->bounds.move(0, -diff);
			}
			fillBounds.move(0, -diff);
		} else if ((top+queue.size()) >= data.size() && fillBounds.bottom < bounds.bottom) {
			// if the top is good but the bottom is not, fix the bottom
			int diff = bounds.bottom-fillBounds.bottom;
			for (std::shared_ptr<Element> &element : queue) {
				element->bounds.move(0, diff);
			}
			fillBounds.move(0, diff);
		}

		// While fillBounds.bottom is smaller than bounds.bottom and there is data left
		while (fillBounds.bottom < bounds.bottom && (top+queue.size()) < data.size()) {
			int index = top+queue.size();
			const Data &data = this->data.at(index);
			int elementSize = this->elementSize; // TODO: Call getElementSize
			Bounds currentBottom = queue.empty() ? Bounds() : queue.back()->bounds;
			Bounds elementBounds(LRTB, bounds.left, bounds.right, currentBottom.bottom, currentBottom.bottom+elementSize);

			// Construct the element and add it to the back
			std::shared_ptr<Element> element = std::make_shared<Element>(data);
			element->bounds = elementBounds;
			elementLayer.add(element);
			queue.push_back(element);

			// Extend fill bounds
			fillBounds.extend(elementBounds);
		}

		// While fillBounds.top is larger than bounds.top and there is data left
		while (fillBounds.top > bounds.top && top > 0) {
			int index = top-1;
			const Data &data = this->data.at(index);
			int elementSize = this->elementSize; // TODO: Call getElementSize
			Bounds currentTop = queue.empty() ? // The queue should never be empty at this point, but things happen
				Bounds(0, elementSize, 0, 0) : queue.front()->bounds;
			Bounds elementBounds(LRTB, bounds.left, bounds.right, currentTop.top-elementSize, currentTop.top);

			// Construct the element and add it to the back
			std::shared_ptr<Element> element = std::make_shared<Element>(data);
			element->bounds = elementBounds;
			elementLayer.add(element);
			queue.push_front(element);

			// Extend fill bounds
			fillBounds.extend(elementBounds);
			top--;
		}

		// If it couldn't add enough to fill the screen...
		if (fillBounds.top > bounds.top) {
			// move all elements up
			int diff = fillBounds.top-bounds.top;
			for (std::shared_ptr<Element> &element : queue) {
				element->bounds.move(0, -diff);
			}
			fillBounds.move(0, -diff);
		} else if (fillBounds.height() > bounds.height() && fillBounds.bottom < bounds.bottom) {
			// if the list can scroll and the bottom is higher than the layout bottom
			int diff = bounds.bottom-fillBounds.bottom;
			for (std::shared_ptr<Element> &element : queue) {
				element->bounds.move(0, diff);
			}
			fillBounds.move(0, diff);
		}
	}

	virtual void getLayers(std::function<void(Layer*)> callback) override {
		callback(&elementLayer);
		callback(&scrollLayer);
	}
};

}
}
