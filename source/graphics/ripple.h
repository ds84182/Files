#pragma once

#include "animation/controller.h"

#include "utils/bounds.h"

#include "circle.h"

namespace GFX {

class Ripple : Circle {
public:
	float hotspotX = 0.0f;
	float hotspotY = 0.0f;
	float centerX = 0.0f;
	float centerY = 0.0f;
	float radiusEnd = 0.0f;
	GFX::Color baseColor;

	Ripple(GFX::Color color = GFX::Color(0,0,0)) :
		Circle(0, 0, 0, color), baseColor(color) {}

	void startEntry();
	void startExit();
	void cancel() { controller.clear(); }
	bool running() {return controller.hasTag("radius") && controller.getTag("radius")->state == Animation::State::Running;}
	void render(float deltaTime);
	
	void onTouchStart(int x, int y) {
		setHotspot(x, y);
		startEntry();
	}

	void onTouchEnd(int x, int y) {
		setHotspot(x, y);
		startExit();
	}
	
	void setHotspot(int x, int y) {
		hotspotX = x;
		hotspotY = y;
	}
	
	void setBounds(Bounds bounds) {
		centerX = bounds.centerX();
		centerY = bounds.centerY();
		radiusEnd = bounds.radius();
	}
	
	void setCenter(Bounds bounds) {
		centerX = bounds.centerX();
		centerY = bounds.centerY();
	}

private:
	float radiusWeight = 0.0f;
	float xWeight = 0.0f;
	float yWeight = 0.0f;
	float opacity = 0.26f;
	Animation::Controller controller;
};

}
