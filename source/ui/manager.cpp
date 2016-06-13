#include "manager.h"

#include <3ds.h>
#include <citro3d.h>

#include <cstdio>
#include <algorithm>
#include <vector>

#include <activity/manager.h>

#include <graphics/core.h>

#include <utils/math_ext.h>

namespace UI {
namespace Manager {

static std::vector<UI::Layer*> layers;

void Add(UI::Layer *layer) {
	layers.push_back(layer);
}

void Remove(UI::Layer *layer) {
	layers.erase(std::remove(layers.begin(), layers.end(), layer), layers.end());
}

static std::weak_ptr<UI::ElementBase> weakFocused;
static touchPosition startTouch;
static touchPosition lastTouch;
static bool tapValid;

void UpdateTemporaryFunctionToUpdateTheUIBecauseUIThreadDoesntExistYet(float delta) {
	ActivityManager::current->dispatchUpdate(delta);

	auto focused = weakFocused.lock();

	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();
	u32 kUp = hidKeysUp();

	if (kDown != 0) ActivityManager::current->dispatchKeyPressed(kDown);
	if (kUp != 0) ActivityManager::current->dispatchKeyReleased(kUp);

	touchPosition touch;
	hidTouchRead(&touch);

	if ((!focused) && (kDown & KEY_TOUCH)) {
		focused.reset();

		// Find an element to focus
		auto layerit = layers.rbegin();
		auto layeritend = layers.rend();
		for (;layerit != layeritend; ++layerit) {
			auto &layer = *layerit;

			focused = layer->find([&](auto &element) {
				return element->bounds.contains(touch.px, touch.py) &&
					element->onTouchStart && element->onTouchStart(touch.px, touch.py);
			});

			if (focused) break;
		}

		if (focused) {
			printf("Focused on element %p\n", focused);
			weakFocused = focused;
			startTouch = touch;
			lastTouch = touch;
			tapValid = true;
		}
	} else if (focused) {
		if (kUp & KEY_TOUCH) { // On key up touch is invalid (0, 0)
			if (tapValid && focused->onTap) { // Tap is invalid if it ever strays away from the 5px requirement
				focused->onTap(lastTouch.px, lastTouch.py);
			}

			if (focused->onTouchEnd) {
				focused->onTouchEnd(lastTouch.px, lastTouch.py);
			}

			weakFocused.reset();
		} else {
			if (tapValid) {
				s32 touchDiffX = ((s32)startTouch.px)-((s32)lastTouch.px);
				s32 touchDiffY = ((s32)startTouch.py)-((s32)lastTouch.py);
				if (std::abs(touchDiffX+touchDiffY) > 7) { // ~5 pixels away in each possible direction
					tapValid = false;
				}
			}

			// Only send onTouchMove if we are not eligible for onTap
			if (!tapValid) {
				if (focused->onTouchMove) {
					focused->onTouchMove(touch.px, touch.py);
				}
			}

			lastTouch = touch;
		}
	}
}

static C3D_Mtx compostMatrix;

void Compost(float timeDelta) {
	auto layerit = layers.begin();
	auto layeritend = layers.end();
	for (;layerit != layeritend; ++layerit) {
		auto &layer = *layerit;
		if (layer->compost) {
			Mtx_Ortho(&compostMatrix, 0.0, float(layer->compostFB.width), float(layer->compostFB.height), 0.0, 0.0, 1.0);
			GFX::DrawOn(&layer->compostFB, &compostMatrix, true);
			layer->render(timeDelta, true);
		}
	}
}

void Render(float timeDelta) {
	auto layerit = layers.begin();
	auto layeritend = layers.end();
	for (;layerit != layeritend; ++layerit) {
		auto &layer = *layerit;
		layer->render(timeDelta);
	}
}

}
}
