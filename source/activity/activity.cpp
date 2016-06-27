#include "manager.h"

#include <deque>
#include <memory>

#include <ui/layer.h>
#include <ui/layout.h>

void Activity::dispatchStart() {
	handler->postCallback([=]() {
		this->onStart();
	});
		this->showLayers();
}

void Activity::dispatchFinish() {
	handler->postCallback([=]() {
		this->onFinish();
	});
}

void Activity::dispatchUpdate(float delta) {
	//handler->postCallback([=]() {
		this->onUpdate(delta);
	//});
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

void Activity::onStart() {
	ActivityManager::HidePrevious();
}

void Activity::onFinish() {
	finish();
}

void Activity::onUpdate(float delta) {}

void Activity::onKeyPressed(u32 keys) {}
void Activity::onKeyReleased(u32 keys) {}

void Activity::addLayer(UI::Layer *layer) {
	layers.push_back(layer);
	layerGroup.add(layer);
}

void Activity::addLayout(UI::Layout &layout) {
	layout.getLayers([&](UI::Layer *layer) {
		addLayer(layer);
	});
}

void Activity::removeLayer(UI::Layer *layer) {
	layers.erase(std::remove(layers.begin(), layers.end(), layer), layers.end());
	layerGroup.remove(layer);
}

void Activity::hideLayers() {
	layerGroup.disabled = true;
}

void Activity::showLayers() {
	layerGroup.disabled = false;
}

void Activity::disableLayers() {
	layerGroup.disabled = true;
	layerGroup.overrideDisableForRender = true;
}

void Activity::finish() {
	this->hideLayers();
	finishing = true;
	ActivityManager::FinishNow();
	handler->stopSafe();
	delete this;
}

namespace ActivityManager {

Activity *current = nullptr;
std::deque<Activity*> stack;

}
