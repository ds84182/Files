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
	virtual ~Activity() {};

	void dispatchStart();
	void dispatchFinish();
	void dispatchUpdate(float delta);
	void dispatchKeyPressed(u32 keys);
	void dispatchKeyReleased(u32 keys);

protected:
	std::shared_ptr<Handler> handler;

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
protected:
	std::vector<UI::Layer*> layers; // Do not modify directly
private:
	bool shown = true;
	bool finishing = false;
};
