#include "recycler_layout.hpp"

namespace UI {
namespace Layouts {

    RecyclerLayout::RecyclerLayout(Adapter *adapter) : adapter(adapter) {
        adapter->layout = this;
        scrollListener = new ScrollListenerElement(this);
        scrollLayer.add(scrollListener);
    }

    RecyclerLayout::~RecyclerLayout() {
        scrollLayer.clear();
        elementLayer.clear();
    }

    int RecyclerLayout::onScroll(int amount) {
        for (auto &element : queue) {
            element->bounds.move(0, amount);
        }

        update();
        return scrollFeedbackAmount;
    }

    void RecyclerLayout::forwardTouchStart(int x, int y) {
        focused = elementLayer.findAt(x, y);

        auto element = focused;
        if (element && element->onTouchStart) {
            element->onTouchStart(x, y);
        }
    }

    void RecyclerLayout::forwardTouchMove(int x, int y) {
        auto element = focused;
        if (element && element->onTouchMove) {
            element->onTouchMove(x, y);
        }
    }

    void RecyclerLayout::forwardTouchEnd(int x, int y) {
        auto element = focused;
        if (element && element->onTouchEnd) {
            element->onTouchEnd(x, y);
        }
    }

    void RecyclerLayout::forwardTap(int x, int y) {
        if (auto element = focused) {
            if (element->onTap) {
                element->onTap(x, y);
            }

            if (onSelected) {
                int index = top;
                for (auto &e : queue) {
                    if (element == e) break;
                    index++;
                }

                onSelected(index);
            }
        }
    }

    void RecyclerLayout::clear() {
        queue.clear();
        elementLayer.clear();
        top = 0;
        focused = nullptr;
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
    void RecyclerLayout::update() {
        // Update bounds for layers and the scroll listener
        elementLayer.bounds = bounds;
        scrollLayer.bounds = bounds;
        scrollListener->bounds = bounds;
        scrollFeedbackAmount = 0;

        // Remove outside elements at the bottom
        while (!queue.empty()) {
            ElementBase *element = queue.back();
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
            ElementBase *element = queue.front();
            if (!element->bounds.contains(bounds)) {
                elementLayer.remove(element);
                queue.pop_front();
                top++;
            } else {
                // We expect every element below this to be inside
                break;
            }
        }

        // Attempt to fit new elements into the view
        Bounds fillBounds(LRTB, std::numeric_limits<int>::max(), std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), std::numeric_limits<int>::min());
        for (ElementBase *element : queue) {
            fillBounds.extend(element->bounds);
        }

        if (top == 0 && fillBounds.top > bounds.top) {
            // move all elements up
            int diff = bounds.top-fillBounds.top;
            for (ElementBase *element : queue) {
                element->bounds.move(0, diff);
            }
            fillBounds.move(0, diff);
            scrollFeedbackAmount = -diff;
        } else if ((top+queue.size()) >= adapter->count() && fillBounds.bottom < bounds.bottom) {
            // if the top is good but the bottom is not, fix the bottom
            int diff = bounds.bottom-fillBounds.bottom;
            for (ElementBase *element : queue) {
                element->bounds.move(0, diff);
            }
            fillBounds.move(0, diff);
            scrollFeedbackAmount = -diff;
        }

        // While fillBounds.bottom is smaller than bounds.bottom and there is data left
        while (fillBounds.bottom < bounds.bottom && (top+queue.size()) < adapter->count()) {
            int index = top+queue.size();
            int elementSize = this->elementSize; // TODO: Call getElementSize
            Bounds currentBottom = queue.empty() ? Bounds() : queue.back()->bounds;
            Bounds elementBounds(LRTB, bounds.left, bounds.right, currentBottom.bottom, currentBottom.bottom+elementSize);

            // Construct the element and add it to the back
            ElementBase *element = adapter->createElement();
            adapter->bindElement(element, index);
            element->bounds = elementBounds;
            elementLayer.add(element);
            queue.push_back(element);

            // Extend fill bounds
            fillBounds.extend(elementBounds);
        }

        // While fillBounds.top is larger than bounds.top and there is data left
        while (fillBounds.top > bounds.top && top > 0) {
            int index = top-1;
            int elementSize = this->elementSize; // TODO: Call getElementSize
            Bounds currentTop = queue.empty() ? // The queue should never be empty at this point, but things happen
                Bounds(0, elementSize, 0, 0) : queue.front()->bounds;
            Bounds elementBounds(LRTB, bounds.left, bounds.right, currentTop.top-elementSize, currentTop.top);

            // Construct the element and add it to the back
            ElementBase *element = adapter->createElement();
            adapter->bindElement(element, index);
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
            int diff = bounds.top-fillBounds.top;
            for (ElementBase *element : queue) {
                element->bounds.move(0, diff);
            }
            fillBounds.move(0, diff);
            scrollFeedbackAmount = -diff;
        } else if (fillBounds.height() > bounds.height() && fillBounds.bottom < bounds.bottom) {
            // if the list can scroll and the bottom is higher than the layout bottom
            int diff = bounds.bottom-fillBounds.bottom;
            for (ElementBase *element : queue) {
                element->bounds.move(0, diff);
            }
            fillBounds.move(0, diff);
            scrollFeedbackAmount = -diff;
        }
    }

    void RecyclerLayout::getLayers(std::function<void(Layer*)> callback) {
        callback(&elementLayer);
        callback(&scrollLayer);
    }

}
}
