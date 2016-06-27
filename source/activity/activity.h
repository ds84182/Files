#pragma once

#include <algorithm>
#include <functional>

#include "ui/layer_group.h"
#include "ui/manager.h"

#include "utils/handler.h"

namespace UI {
	class Layer;
	class Layout;
}

class Activity {
public:
	Activity() {} // TODO: onCreate with saved data
	virtual ~Activity() {};

	void dispatchStart();
	void dispatchFinish();
	void dispatchUpdate(float delta);
	void dispatchKeyPressed(u32 keys);
	void dispatchKeyReleased(u32 keys);

	void hideLayers();
	void showLayers();
	void forEachLayer(std::function<void(UI::Layer*)> callback) {
		for (auto layer : layers) callback(layer);
	}
	void disableLayers();

protected:
	std::vector<UI::Layer*> layers; // Do not modify directly

	virtual void onStart();
	virtual void onFinish();
	// THIS DOES NOT GET CALLED ON THE HANDLER THREAD! BEWARE!
	virtual void onUpdate(float delta);
	virtual void onKeyPressed(u32 keys);
	virtual void onKeyReleased(u32 keys);

	void addLayer(UI::Layer *layer);
	void addLayout(UI::Layout &layout);
	void removeLayer(UI::Layer *layer);

	void finish();
private:
	UI::LayerGroup layerGroup;
	bool shown = false;
	bool finishing = false;
};
