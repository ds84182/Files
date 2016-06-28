#pragma once

namespace UI {

class LayerGroup;

namespace Manager {

extern LayerGroup *RootLayerGroup;

void Update(float delta);

void Compost(float timeDelta);
void Render(float timeDelta);

}
}
