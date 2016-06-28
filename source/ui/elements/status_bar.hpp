#pragma once

#include <3ds.h>

#include <graphics/core.hpp>
#include <graphics/fonts.hpp>

#include <ui/element.hpp>

namespace UI {
namespace Elements {

struct StatusBarData {
    std::u32string systemTime = U"12:00";
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
