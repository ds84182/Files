#pragma once

#include <vector>

#include "layer.hpp"

namespace UI {

class LayerGroup {
public:
    LayerGroup *next = nullptr;
    LayerGroup *prev = nullptr;
    std::vector<Layer*> layers;
    bool disabled = false;
    bool overrideDisableForRender = false;

    LayerGroup();
    LayerGroup(LayerGroup *relative, bool before = true);
    ~LayerGroup();

    void add(Layer *layer) {
        layers.push_back(layer);
    }

    void remove(Layer *layer);

    void compost(float delta);
    void render(float delta);

    ElementBase *find(std::function<bool(ElementBase*)> func) {
        if (disabled) return nullptr;

		auto layerit = layers.rbegin();
		auto layeritend = layers.rend();
		for (;layerit != layeritend; ++layerit) {
			auto layer = *layerit;
			auto element = layer->find(func);
			if (element) return element;
		}

		return nullptr;
	}

	ElementBase *findAt(int x, int y) {
		return find([&](auto element) {
			return element->bounds.contains(x, y);
		});
	}
};

}
