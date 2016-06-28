#include "status_bar.hpp"

#include <malloc.h>

#include <graphics/rectangle.hpp>
#include <graphics/scissor.hpp>

#include <style/color.hpp>

UI::Elements::StatusBarData UI::Elements::StatusBar::Data;

extern u32 __ctru_heap_size;
extern u32 __ctru_linear_heap_size;

void UI::Elements::StatusBar::init() {
    bounds = Bounds(400, 24);
}

static void convertToU32(const char *str, std::u32string &dest) {
    // Convert the given utf8 string into utf32
    ssize_t len = utf8_to_utf32(nullptr, (const u8*)str, 0);
    dest = std::move(std::u32string(len, '\0'));
    utf8_to_utf32((u32*)dest.data(), (const u8*)str, dest.length());
}

char memUsageBuffer[1024] = {0};

void UI::Elements::StatusBar::render(float timeDelta) {
    GFX::Scissor scissor(bounds);

    GFX::Rectangle rect(bounds.left, bounds.top, bounds.width(), bounds.height(), Style::colorPrimaryDark);
    rect.render();

    u64 currentTime = osGetTime() / 1000 / 60;
    if (Data.systemTime.empty() || currentTime != Data.systemTimeMin) {
        Data.systemTimeMin = currentTime;
        // Convert currentTime into number of minutes in the current day
        currentTime %= 60*24;
        // Seperate into components
        u8 minutes = (currentTime % 60);
        u8 hours = currentTime / 60;
        // Modify for 12 hour clock
        if (hours == 0) {
            hours = 12;
        } else if (hours >= 13) {
            hours -= 12;
        }
        sprintf(memUsageBuffer, "%02d:%02d", hours, minutes);
        convertToU32(memUsageBuffer, Data.systemTime);
    }

    auto &font = GFX::Fonts::CaptionMedium;

    int textWidth = font.width(Data.systemTime);
    int textHeight = font.height();
    font.drawText(Data.systemTime, bounds.right-textWidth-8, 12-textHeight/2, GFX::Color(255,255,255));

    struct mallinfo mi = mallinfo();
    sprintf(memUsageBuffer, "Linear: %d/%d; VRAM: %d/%d",
        __ctru_linear_heap_size-linearSpaceFree(), __ctru_linear_heap_size,
        0x600000-vramSpaceFree(), 0x600000);
        //mi.uordblks, __ctru_heap_size);
    std::u32string memUsage;
    convertToU32(memUsageBuffer, memUsage);
    font.drawText(memUsage, 8, 12-textHeight/2, GFX::Color(255,255,255));
}
