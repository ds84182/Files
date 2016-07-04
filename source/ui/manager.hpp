#pragma once

namespace UI {

class LayerGroup;
class ElementBase;

namespace Manager {

extern LayerGroup *RootLayerGroup;

ElementBase *GetFocusedElement();
void ClearFocusedElement();

void Update(float delta);

void Compost(float timeDelta);
void Render(float timeDelta);

}
}
