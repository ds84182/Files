#pragma once

#include <3ds.h>

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <vector>

#include <graphics/core.hpp>
#include <graphics/framebuffer.hpp>
#include <graphics/rectangle.hpp>
#include <graphics/scissor.hpp>
#include <graphics/texture.hpp>

#include "element.hpp"

#include <utils/bounds.hpp>

namespace UI {

namespace Manager {
	void Compost(float);
}

class LayerGroup;

/*
Layers manage a list of Elements. This is the only way to group multiple elements together.

Layers can also do render clipping. Layers have bounds too.
Layers can also be composted.

Composition Rules:
Composition is not automatic. You must request a layer to be composted the next frame.
The compost framebuffer is not automatically removed after modifying an element.
Layer animations will request the layer to be composted.
A layer can be set to be actively composted every frame. This is only useful to show layer animations
	WHILE showing element animations.
*/
class Layer {
public:
	Bounds bounds;

	float x = 0, y = 0, scaleX = 1, scaleY = 1;
	bool topScreen = false;

	bool hasBackground = false;
	GFX::Color backgroundColor;
	GFX::Texture *backgroundTexture = nullptr;
	GFX::Color compostColor = GFX::Color(255, 255, 255);

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

	void render(float timeDelta, bool duringCompost = false) {
		if (topScreen && GFX::currentScreen == GFX::Screen::Bottom) return;
		if ((!topScreen) && GFX::currentScreen != GFX::Screen::Bottom) return;

		if (!duringCompost) {
			auto mtx = GFX::PushMatrix();

			Mtx_Translate(mtx, x, y, 0);
			Mtx_Scale(mtx, scaleX, scaleY, 1);

			GFX::UpdateMatrix();
		}

		if (compost && !duringCompost) {
			// Layer scale transforms only work when composted
			GFX::Scissor scissor(bounds);
			GFX::Texture tex = compostFB.getColorTexture();
			GFX::Rectangle rect(bounds.left, bounds.top,
				bounds.width(), bounds.height(), compostColor);
			rect.render(&tex, bounds.width(), bounds.height());
		} else {
			lock();
			// TODO: Transform bounds and layer (if not duringCompost)
			GFX::Scissor scissor(bounds);
			// This scissors (or subscissors) an area
			// Scissors can be applied recursively

			if (hasBackground) {
				GFX::Rectangle rect(bounds.left, bounds.top, bounds.width(), bounds.height(), backgroundColor);
				rect.render(backgroundTexture);
			}

			std::for_each(elements.rbegin(), elements.rend(), [&](auto &element) {
				element->render(timeDelta);
			});

			unlock();
		}

		if (!duringCompost) {
			GFX::PopMatrix();
		}
	}

	void startCompost(bool alwaysDirty = false, bool withAlpha = false, bool withStencil = false) {
		compost = true;
		compostDirty = true;
		compostAlwaysDirty = alwaysDirty;

		compostFB.destroy();
		compostFB.create(GFX::np2(bounds.width()), GFX::np2(bounds.height()),
			withAlpha ? GPU_RB_RGBA8 : GPU_RB_RGB8,
			withStencil ? GPU_RB_DEPTH24_STENCIL8 : GPU_RB_DEPTH16);
		compostFB.setClear(C3D_CLEAR_ALL, GFX::Color(0, 0, 0, 0));
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

	// TODO: Make this private
	std::vector<std::shared_ptr<ElementBase>> elements; // Do not access this directly without locking
	// TODO: Layer should have event listeners (so the RecyclerLayout can remove the scroll listener element)
	// This would require a base class for every object that listens to element events

	bool compost = false; // Render to framebuffer and use the framebuffer for rendering
	bool compostDirty = true;
	bool compostAlwaysDirty = false; // Always render to framebuffer every frame
	GFX::FrameBuffer compostFB;

	friend class UI::LayerGroup;
};

}
