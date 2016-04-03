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

}
