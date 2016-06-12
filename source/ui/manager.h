#pragma once

#include <memory>

#include "ui/element.h"
#include "ui/layer.h"

namespace UI {
namespace Manager {

void Add(UI::Layer *layer);
void Remove(UI::Layer *layer);

// Temporary function to update the UI because UI thread isn't a thing yet
void UpdateTemporaryFunctionToUpdateTheUIBecauseUIThreadDoesntExistYet();

void Compost(float timeDelta);
void Render(float timeDelta);

}
}
