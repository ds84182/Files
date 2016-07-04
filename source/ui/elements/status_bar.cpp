#include "status_bar.hpp"

#include <malloc.h>

#include <graphics/rectangle.hpp>
#include <graphics/scissor.hpp>

#include <fmt/format.hpp>

UI::Elements::StatusBarData UI::Elements::StatusBar::Data;

extern u32 __ctru_heap_size;
extern u32 __ctru_linear_heap_size;

void UI::Elements::StatusBar::init() {
    bounds = Bounds(400, 24);
}

static void convertToU32(const char *str, std::wstring &dest) {
    // Convert the given utf8 string into utf32
    ssize_t len = utf8_to_utf32(nullptr, (const u8*)str, 0);
    dest = std::move(std::wstring(len, '\0'));
    utf8_to_utf32((u32*)dest.data(), (const u8*)str, dest.length());
}

char memUsageBuffer[1024] = {0};

void UI::Elements::StatusBar::render(float timeDelta) {
    GFX::Scissor scissor(bounds);

    GFX::Rectangle rect(bounds.left, bounds.top, bounds.width(), bounds.height(), Data.primaryDark);
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

    scissor.exit();

    fmt::WMemoryWriter debugWriter;

    debugWriter << L"Linear: " <<
        __ctru_linear_heap_size-linearSpaceFree() << L'/' <<
        __ctru_linear_heap_size << L'\n';

    debugWriter << L"VRAM: " <<
        0x600000-vramSpaceFree() << L'/' <<
        0x600000 << L'\n';

    debugWriter << L"Normal: " <<
        mallinfo().uordblks << L'/' <<
        __ctru_heap_size << L'\n';

    debugWriter << Data.debugText;

    std::wstring debug = debugWriter.str();
    font.drawText(debug, 8, bounds.height()+8, GFX::Color(255,255,255));
}
