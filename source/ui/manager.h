#pragma once

namespace UI {

class LayerGroup;

namespace Manager {

extern LayerGroup *RootLayerGroup;

// Temporary function to update the UI because UI thread isn't a thing yet
void UpdateTemporaryFunctionToUpdateTheUIBecauseUIThreadDoesntExistYet(float delta);

void Compost(float timeDelta);
void Render(float timeDelta);

}
}
