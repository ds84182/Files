#pragma once

#include <functional>
#include <memory>

#include "ui/layer.h"
#include "utils/bounds.h"

namespace UI {

class Layout {
public:
	Bounds bounds;

	virtual ~Layout() = default;
	virtual void update() = 0;
	virtual void getLayers(std::function<void(Layer*)> callback);
};

}
