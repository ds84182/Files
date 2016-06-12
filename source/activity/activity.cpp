#include "manager.h"

#include <deque>
#include <memory>

void Activity::dispatchStart() {
	handler->postCallback([=]() {
		this->onStart();
	});
}

void Activity::dispatchFinish() {
	handler->postCallback([=]() {
		this->onFinish();
	});
}

void Activity::dispatchKeyPressed(u32 keys) {
	handler->postCallback([=]() {
		this->onKeyPressed(keys);
	});
}

void Activity::dispatchKeyReleased(u32 keys) {
	handler->postCallback([=]() {
		this->onKeyReleased(keys);
	});
}

void Activity::onStart() {}

void Activity::onFinish() {
	finish();
}

void Activity::onKeyPressed(u32 keys) {}
void Activity::onKeyReleased(u32 keys) {}

void Activity::addLayer(UI::Layer *layer) {
	layers.push_back(layer);
	if (shown) {
		UI::Manager::Add(layer);
	}
}

void Activity::addLayout(UI::Layout &layout) {
	layout.getLayers([&](UI::Layer *layer) {
		addLayer(layer);
	});
}

void Activity::removeLayer(UI::Layer *layer) {
	layers.erase(std::remove(layers.begin(), layers.end(), layer), layers.end());
	if (shown) {
		UI::Manager::Remove(layer);
	}
}

namespace ActivityManager {

std::unique_ptr<Activity> current;
std::deque<std::unique_ptr<Activity>> stack;

}
