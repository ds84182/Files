#include "status_bar.hpp"

#include <graphics/rectangle.h>
#include <graphics/scissor.h>

#include <style/color.hpp>

UI::Elements::StatusBarData UI::Elements::StatusBar::Data;

void UI::Elements::StatusBar::init() {
    bounds = Bounds(400, 24);
}

void UI::Elements::StatusBar::render(float timeDelta) {
    GFX::Scissor scissor(bounds);

    GFX::Rectangle rect(bounds.left, bounds.top, bounds.width(), bounds.height(), Style::colorPrimaryDark);
    rect.render();

    int textWidth = GFX::noto.width(Data.systemTime);
    int textHeight = GFX::noto.height();
    // TODO: GFX::Color::Black
    GFX::noto.drawText(Data.systemTime, bounds.right-textWidth-8, 12+(textHeight/2), GFX::Color(255,255,255));
}
