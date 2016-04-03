#pragma once

#include <3ds.h>

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <vector>

#include "graphics/scissor.h"

#include "element.h"
#include "utils/bounds.h"

namespace UI {

/*
Layers manage a list of Elements. This is the only way to group multiple elements together.

Layers can also do render clipping. Layers have bounds too.
Layers can also be composted.
*/
class Layer {
public:
	Bounds bounds;
	std::vector<std::shared_ptr<ElementBase>> elements; // Do not access this directly without locking

	Layer() {
		LightLock_Init(&lck);
	}

	Layer(std::initializer_list<std::shared_ptr<ElementBase>> list) : elements(list) {
		LightLock_Init(&lck);
	}

	void lock() {
		LightLock_Lock(&lck);
	}

	void unlock() {
		LightLock_Unlock(&lck);
	}

	void add(const std::shared_ptr<ElementBase> &element) {
		lock();
		elements.push_back(element);
		unlock();
	}

	void remove(const std::shared_ptr<ElementBase> &element) {
		lock();
		elements.erase(std::remove(elements.begin(), elements.end(), element), elements.end());
		unlock();
	}

	void clear() {
		lock();
		elements.clear();
		unlock();
	}

	void render(float timeDelta) {
		lock();
		GFX::Scissor scissor(bounds); // This scissors (or subscissors) an area
		// Scissors can be applied recursively
		std::for_each(elements.rbegin(), elements.rend(), [&](auto &element) {
			element->render(timeDelta);
		});

		unlock();
	}

	std::shared_ptr<ElementBase> find(std::function<bool(std::shared_ptr<ElementBase>&)> func) {
		lock();
		auto it = std::find_if(elements.rbegin(), elements.rend(), func);
		unlock();

		if (it == elements.rend()) {
			return std::shared_ptr<ElementBase>();
		} else {
			return *it;
		}
	}

	std::shared_ptr<ElementBase> findAt(int x, int y) {
		return find([&](auto &element) {
			return element->bounds.contains(x, y);
		});
	}
private:
	LightLock lck;
};

}
