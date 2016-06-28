#include "layer_group.hpp"

#include <citro3d.h>

#include <algorithm>
#include <cstdio>

#include "manager.hpp"

#include <graphics/core.hpp>

#include <utils/math_ext.hpp>

UI::LayerGroup::LayerGroup() {
    if (UI::Manager::RootLayerGroup == nullptr) {
        UI::Manager::RootLayerGroup = this;
    } else {
        auto current = UI::Manager::RootLayerGroup;
        while (current->next) current = current->next;
        current->next = this;
        prev = current;
    }
}

UI::LayerGroup::LayerGroup(LayerGroup *relative, bool before) {
    if (before) {
        if (!relative->prev) {
            UI::Manager::RootLayerGroup = this;
        }
        this->prev = relative->prev;
        relative->prev = this;
        this->next = relative;
    } else {
        this->next = relative->next;
        relative->next = this;
        this->prev = relative;
    }
}

UI::LayerGroup::~LayerGroup() {
    if (next) {
        next->prev = prev;
    }
    if (prev) {
        prev->next = next;
    }

    if (UI::Manager::RootLayerGroup == this) {
        UI::Manager::RootLayerGroup = next;
    }
}

void UI::LayerGroup::remove(Layer *layer) {
    layers.erase(std::remove(layers.begin(), layers.end(), layer), layers.end());
}

static C3D_Mtx compostMatrix;

void UI::LayerGroup::compost(float timeDelta) {
    if (disabled && !overrideDisableForRender) return;

    for (auto layer : layers) {
        if (layer->compost) {
			Mtx_Ortho(&compostMatrix, 0.0, float(layer->compostFB.width), float(layer->compostFB.height), 0.0, 0.0, 1.0);
			GFX::DrawOn(&layer->compostFB, &compostMatrix, true);
			layer->render(timeDelta, true);
		}
    }
}

void UI::LayerGroup::render(float timeDelta) {
    if (disabled && !overrideDisableForRender) return;

    for (auto layer : layers) {
        layer->render(timeDelta);
    }
}
