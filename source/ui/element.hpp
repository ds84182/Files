#pragma once

#include <functional>

#include <utils/bounds.hpp>

namespace UI {

class ElementBase {
public:
	Bounds bounds;
	std::function<bool(int,int)> onTap; // < Called after onTouchEnd if the distance between the start is <5px
	std::function<bool(int,int)> onTouchStart;
	std::function<bool(int,int)> onTouchMove;
	std::function<bool(int,int)> onTouchEnd;

	virtual ~ElementBase();
	virtual void render(float timeDelta) = 0;
};

// Elements cannot have sub elements. This is done to simplify the rendering and input detection.
// To lay out elements, use a Layout.

// The UI has layers.
// For example, ScrollLayout puts all visible item elements on the base layer and puts the ScrollListenerElement
// on the layer above it

// Elements have data attached to them. Elements should make a copy of the data into itself instead
// of refering to the same data. This is so that data can be modified on a thread and pushed into
// the element in a thread safe fashion.
// Element data must have a default constructor that makes it in an invalid state and a copy constructor.

template <typename T>
class Element : public ElementBase {
public:
	using Data = T;

	Element() {}
	Element(const T &data) : data(data) {}
	virtual ~Element() = default;

	void setData(T &data) {
		// TODO: Lock
		this->data = data;
		// TODO: Unlock
	}

protected:
	T data;
};

}
