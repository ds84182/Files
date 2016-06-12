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

protected:
	std::shared_ptr<Handler> handler;

	virtual void onStart();
	virtual void onFinish();

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
private:
	std::vector<UI::Layer*> layers;
	bool shown = true;
	bool finishing = false;
};
