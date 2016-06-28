#include "ripple.hpp"

// Constants and formulas [stolen] from Android Lollipop
const float waveTouchDownAcceleration = 1024.0f;
const float waveTouchUpAcceleration = 3400.0f;
const float waveOpacityDecayVelocity = 3.0f;

const float rippleEnterDelay = 80.0f/1000.0f;

// 3DS top and bottom screen have an average density of ~113dpi
// This is classified as "lowdpi" (which rounds up to 120dpi)
// 1 px = 1 dp at 160 dpi
const float density = 120.0f/160.0f;

using Animation::LinearInterpolator;
using Animation::LogDecelerationInterpolatorFactory;
using Animation::ValueTarget;

void GFX::Ripple::startEntry() {
	cancel();

	const float radiusEnterDuration = sqrt(radiusEnd / waveTouchDownAcceleration * density) + 0.5f;
	controller.start(LinearInterpolator, ValueTarget(&radiusWeight, 0.0f, 1.0f), radiusEnterDuration,
		nullptr, "radius", -rippleEnterDelay);
	controller.start(LinearInterpolator, ValueTarget(&xWeight, 0.0f, 1.0f), radiusEnterDuration,
		nullptr, "x", -rippleEnterDelay);
	controller.start(LinearInterpolator, ValueTarget(&yWeight, 0.0f, 1.0f), radiusEnterDuration,
		nullptr, "y", -rippleEnterDelay);
	opacity = 0.26f;
}

static inline float lerp(float a, float b, float t) {
	return ((b-a)*t)+a;
}

auto Deaccel = LogDecelerationInterpolatorFactory(400.0f, 1.4f);

void GFX::Ripple::startExit() {
	const bool wasRunning = running();
	cancel();

	const float radius = lerp(0, radiusEnd, radiusWeight);
	const float remaining = wasRunning ? radiusEnd - radius : radiusEnd;

	const float radiusExitDuration = sqrt(remaining / (waveTouchUpAcceleration+waveTouchDownAcceleration) * density) + 0.5f;
	const float opacityDuration = opacity / waveOpacityDecayVelocity + 0.5f;

	controller.start(Deaccel, ValueTarget(&radiusWeight, radiusWeight, 1.0f), radiusExitDuration);
	controller.start(Deaccel, ValueTarget(&xWeight, xWeight, 1.0f), radiusExitDuration);
	controller.start(Deaccel, ValueTarget(&yWeight, yWeight, 1.0f), radiusExitDuration);
	controller.start(LinearInterpolator, ValueTarget(&opacity, opacity, 0.0f), opacityDuration);
}

void GFX::Ripple::render(float deltaTime) {
	controller.update(deltaTime);

	color = baseColor;
	color.a = (u8) ((255.0f * opacity) + 0.5f);
	const float radius = lerp(0, radiusEnd, radiusWeight);

	if (color.a > 0 && radius > 0) {
		x = lerp(hotspotX, centerX, xWeight);
		y = lerp(hotspotY, centerY, yWeight);
		this->radius = radius;
		Circle::render();
	}
}
