#pragma once

#include <functional>

#include <graphics/core.hpp>

namespace Animation {

using Target = std::function<void(float)>;

template <typename T>
Target ValueTarget(T *var, T start, T end) { // TODO: Maybe this could be a reference?
	return [=](float time) {
		*var = ((end-start)*time)+start;
	};
}

template <>
inline Target ValueTarget<GFX::Color>(GFX::Color *var, GFX::Color start, GFX::Color end) {
	return [=](float time) {
		*var = start.lerp(end, time);
	};
}

}
