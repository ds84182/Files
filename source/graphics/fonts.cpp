#include "fonts.hpp"

#include <cstdio>

GFX::Font GFX::Fonts::CaptionMedium;
GFX::Font GFX::Fonts::Caption;
GFX::Font GFX::Fonts::Body1;
GFX::Font GFX::Fonts::Body2;
GFX::Font GFX::Fonts::Subhead;
GFX::Font GFX::Fonts::Title;
GFX::Font GFX::Fonts::Headline;

void GFX::Fonts::Init() {
    auto fh = fopen("romfs:/Roboto-Regular.ttf", "rb");
    fseek(fh, 0L, SEEK_END);
    auto robotoRegularSize = ftell(fh);
    fseek(fh, 0L, SEEK_SET);

    u8 *robotoRegular = new u8[robotoRegularSize];
    fread(robotoRegular, 1, robotoRegularSize, fh);
    fclose(fh);

    fh = fopen("romfs:/Roboto-Medium.ttf", "rb");
    fseek(fh, 0L, SEEK_END);
    auto robotoMediumSize = ftell(fh);
    fseek(fh, 0L, SEEK_SET);

    u8 *robotoMedium = new u8[robotoMediumSize];
    fread(robotoMedium, 1, robotoMediumSize, fh);
    fclose(fh);

    CaptionMedium.init(robotoMedium, robotoMediumSize, 12);
    Caption.init(robotoRegular, robotoRegularSize, 12);
    Body1.init(robotoRegular, robotoRegularSize, 14);
    Body2.init(robotoMedium, robotoMediumSize, 14);
    Subhead.init(robotoRegular, robotoRegularSize, 16);
    Title.init(robotoMedium, robotoMediumSize, 20);
    Headline.init(robotoRegular, robotoRegularSize, 24);
}
