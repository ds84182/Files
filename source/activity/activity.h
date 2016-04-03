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
	
	void requestStart() {
		handler->postCallback([=]() {this->onStart();});
	}
	
	void requestFinish() {
		handler->postCallback([=]() {this->onFinish();});
	}

protected:
	std::shared_ptr<Handler> handler;

	virtual void onStart() {};
	virtual void onFinish() {
		finish();
	}

	void addLayer(std::shared_ptr<UI::Layer> layer) {
		layers.push_back(layer);
		if (shown) {
			UI::Manager::Add(layer);
		}
	}

	void addLayout(UI::Layout &layout) {
		layout.getLayers([&](std::shared_ptr<UI::Layer> layer) {
			addLayer(layer);
		});
	}

	void removeLayer(std::shared_ptr<UI::Layer> layer) {
		layers.erase(std::remove(layers.begin(), layers.end(), layer), layers.end());
		if (shown) {
			UI::Manager::Remove(layer);
		}
	}
	
	void finish() {
		shown = false;
		for (auto &layer : layers) {
			UI::Manager::Remove(layer);
		}
		handler->stopSafe();
	}
private:
	std::vector<std::shared_ptr<UI::Layer>> layers;
	bool shown = true;
};
