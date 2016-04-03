#pragma once

#include <memory>

#include "ui/element.h"
#include "ui/layer.h"

namespace UI {
namespace Manager {

void Add(std::shared_ptr<UI::Layer> layer);
void Remove(std::shared_ptr<UI::Layer> layer);

// Temporary function to update the UI because UI thread isn't a thing yet
void UpdateTemporaryFunctionToUpdateTheUIBecauseUIThreadDoesntExistYet();

void Render(float timeDelta);

}
}
