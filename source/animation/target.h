#pragma once

#include <functional>

namespace Animation {

using Target = std::function<void(float)>;

template <typename T>
Target ValueTarget(T *var, T start, T end) { // TODO: Maybe this could be a reference?
	return [=](float time) {
		*var = ((end-start)*time)+start;
	};
}

}
