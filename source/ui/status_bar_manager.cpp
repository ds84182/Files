#include "status_bar_manager.hpp"

#include "layer_group.hpp"
#include "layer.hpp"

#include "elements/status_bar.hpp"

#include <style/color.hpp>

namespace UI {
namespace StatusBarManager {

static UI::Layer topLayer;
static UI::LayerGroup rootLayerGroup;

void Init() {
    topLayer.topScreen = true;
    topLayer.bounds = Bounds(400, 240); // TODO: Some screen size constants
    topLayer.hasBackground = true;
    topLayer.backgroundColor = Style::colorPrimary;
    topLayer.add(std::make_shared<UI::Elements::StatusBar>()); // TODO: Remove shared_ptr bullshit
    rootLayerGroup.add(&topLayer);
}

}
}
