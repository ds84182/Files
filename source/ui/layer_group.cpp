#include "layer_group.h"

#include <citro3d.h>

#include <algorithm>
#include <cstdio>

#include "manager.h"

#include <graphics/core.h>

#include <utils/math_ext.h>

UI::LayerGroup::LayerGroup() {
  if (UI::Manager::RootLayerGroup == nullptr) {
    UI::Manager::RootLayerGroup = this;
  } else {
    auto current = UI::Manager::RootLayerGroup;
    while (current->next) current = current->next;
    current->next = this;
    prev = current;
  }
  printf("Layer Group %p created\n", this);
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

  printf("Layer Group %p deleted\n", this);
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
