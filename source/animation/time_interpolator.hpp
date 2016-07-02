#pragma once

#include <cmath>
#include <functional>

namespace Animation {

using TimeInterpolator = std::function<float(float)>;

float LinearInterpolator(float);

inline TimeInterpolator LogDecelerationInterpolatorFactory(float base, float timeScale) {
	return [=](float input) {
		return 1.0f - std::pow(base, -input * timeScale);
	};
}

inline TimeInterpolator DecelerateInterpolatorFactory(float factor = 1.0f) {
	return [=](float input) {
		if (factor == 1.0f) {
			return 1.0f - (1.0f - input) * (1.0f - input);
		}
		return 1.0f - std::pow((1.0f - input), 2 * factor);
	};
}

float FastOutSlowInInterpolator(float);
float FastOutLinearInInterpolator(float);
float LinearOutSlowInInterpolator(float);

}
