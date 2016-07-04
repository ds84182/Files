#pragma once

#include <3ds.h>

#include <graphics/core.hpp>
#include <graphics/fonts.hpp>

#include <style/color.hpp>

#include <ui/element.hpp>

namespace UI {
namespace Elements {

struct StatusBarData {
    u64 systemTimeMin;
    std::wstring systemTime;
    std::wstring debugText;
    GFX::Color primaryDark = Style::colorPrimaryDark;
};

class StatusBar : public Element<std::nullptr_t> {
public:
    static StatusBarData Data;

    StatusBar(std::nullptr_t np = nullptr) : Element(np) {init();}

    void init();
    virtual void render(float timeDelta) override;
};

}
}
