#pragma once

#include <algorithm>
#include <functional>

#include "ui/layer.h"
#include "ui/layout.h"
#include "ui/manager.h"

#include "utils/handler.h"

class Activity {
public:
	Activity(std::shared_ptr<Handler> handler) : handler(handler) {
		// TODO: onCreate with saved data
	}

	void dispatchStart();
	void dispatchFinish();
	void dispatchKeyPressed(u32 keys);
	void dispatchKeyReleased(u32 keys);

protected:
	std::shared_ptr<Handler> handler;

	virtual void onStart();
	virtual void onFinish();
	virtual void onKeyPressed(u32 keys);
	virtual void onKeyReleased(u32 keys);

	void addLayer(UI::Layer *layer);

	void addLayout(UI::Layout &layout);

	void removeLayer(UI::Layer *layer);

	void finish() {
		shown = false;
		finishing = true;
		for (auto &layer : layers) {
			UI::Manager::Remove(layer);
		}
		handler->stopSafe();
	}
protected:
	std::vector<UI::Layer*> layers; // Do not modify directly
private:
	bool shown = true;
	bool finishing = false;
};
