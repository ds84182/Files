#include "manager.h"

#include <3ds.h>

#include <cstdio>
#include <algorithm>
#include <vector>

#include "layer_group.h"

#include <activity/manager.h>

namespace UI {
namespace Manager {

UI::LayerGroup *RootLayerGroup = nullptr;

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

		// Get last group
		auto group = RootLayerGroup;
		while (group && group->next) {
			group = group->next;
		}

		while (group) {
			focused = group->find([&](auto &element) {
				return element->bounds.contains(touch.px, touch.py) &&
					element->onTouchStart && element->onTouchStart(touch.px, touch.py);
			});

			if (focused) break;
			group = group->prev;
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

void Compost(float timeDelta) {
	auto group = RootLayerGroup;
	while (group) {
		group->compost(timeDelta);
		group = group->next;
	}
}

void Render(float timeDelta) {
	auto group = RootLayerGroup;
	while (group) {
		group->render(timeDelta);
		group = group->next;
	}
}

}
}
