#pragma once

#include <functional>

#include <graphics/core.h>

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
		s32 r = ((end.r-start.r)*time)+start.r;
		s32 g = ((end.g-start.g)*time)+start.g;
		s32 b = ((end.b-start.b)*time)+start.b;
		s32 a = ((end.a-start.a)*time)+start.a;
		*var = GFX::Color(r, g, b, a);
	};
}

}
