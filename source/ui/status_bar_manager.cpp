#include "status_bar_manager.hpp"

#include "layer_group.hpp"
#include "layer.hpp"

#include "elements/status_bar.hpp"

#include <animation/controller.hpp>
#include <animation/target.hpp>
#include <animation/time_interpolator.hpp>

#include <style/color.hpp>

namespace UI {
namespace StatusBarManager {

static UI::Layer topLayer;
static UI::LayerGroup rootLayerGroup;
static std::shared_ptr<UI::Elements::StatusBar> statusBar;
static Animation::Controller anims;
static bool isBlack = false;

void Init() {
    topLayer.topScreen = true;
    topLayer.bounds = Bounds(400, 240); // TODO: Some screen size constants
    topLayer.hasBackground = true;
    topLayer.backgroundColor = Style::colorPrimary;
    topLayer.add(statusBar = std::make_shared<UI::Elements::StatusBar>()); // TODO: Remove shared_ptr bullshit
    rootLayerGroup.add(&topLayer);
}

void Update(float delta) {
    anims.update(delta);
}

void SetBlack(bool black) {
    if (black != isBlack) {
        isBlack = black;

        if (anims.hasTag("statusbar_color")) {
            anims.cancelTag("statusbar_color");
        }

        if (anims.hasTag("background_color")) {
            anims.cancelTag("background_color");
        }

        anims.start(Animation::FastOutSlowInInterpolator,
            Animation::ValueTarget(&statusBar->Data.primaryDark, statusBar->Data.primaryDark,
            black ? GFX::Color(0,0,0) : Style::colorPrimaryDark), 0.225f, nullptr, "statusbar_color");
        anims.start(Animation::FastOutSlowInInterpolator,
            Animation::ValueTarget(&topLayer.backgroundColor, topLayer.backgroundColor,
            black ? GFX::Color(0,0,0) : Style::colorPrimary), 0.225f, nullptr, "background_color");
    }
}

void SetDebugText(std::wstring text) {
    statusBar->Data.debugText = text;
}

}
}
