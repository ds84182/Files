#include "activity.hpp"
#include "manager.hpp"

#include <graphics/fonts.hpp>
#include <graphics/texture.hpp>

#include <fmt/format.hpp>

#include <ui/element.hpp>
#include <ui/status_bar_manager.hpp>

#include <utils/filesystem/path.hpp>
#include <utils/lodepng.hpp>

#include <style/color.hpp>

class ImageViewElement : public UI::ElementBase {
public:
    std::wstring error;
    GFX::Texture texture;
    float zoom = 1, panX = 0, panY = 0;

    void setTexture(const GFX::Texture &tex) {
        if (tex.width > tex.height) {
            zoom = 320.f/tex.width;
            panX = 0;
            panY = 120.f-((tex.height*zoom)/2.f);
        } else {
            zoom = 240.f/tex.height;
            panX = 160.f-((tex.width*zoom)/2.f);
            panY = 0;
        }
        if (zoom > 1.f) {
            zoom = 1.f;
            panX = 160.f-(tex.width/2.f);
            panY = 120.f-(tex.height/2.f);
        }
        texture = tex;
    }

    virtual void render(float timeDelta) override {
        if (texture.tex) {
            GFX::Rectangle rect(panX, panY, texture.width*zoom, texture.height*zoom, GFX::Color(255,255,255));
            rect.render(&texture);
        } else {
            auto &font = GFX::Fonts::Body1;

            int textHeight = font.height(error, 320-16);
            font.drawTextWrap(error, 8, 120-(textHeight/2), 320-16, GFX::Color(255,255,255));
        }
    }
};

class ImageViewActivity : public Activity {
public:
    FS::Path path;
    GFX::Texture texture;
    ImageViewElement *elem = new ImageViewElement();
    UI::Layer imageLayer;
    ImageViewActivity(const FS::Path &path) : path(path) {}

    virtual void onStart() override {
        unsigned char *image;
        unsigned width, height;

        //decode
        unsigned error = lodepng_decode32_file(&image, &width, &height, path.str().c_str());

        if (!error) {
            texture.create(width, height, GPU_RGBA8);
            texture.copyAndTile(reinterpret_cast<u32*>(image), width, height);
            free(image);
            elem->setTexture(texture);
        }

        fmt::WMemoryWriter debugWriter;
        if (error) debugWriter << L"Error " << error << L": " << lodepng_error_text(error);
        UI::StatusBarManager::SetDebugText(elem->error = debugWriter.str());

        UI::StatusBarManager::SetBlack(true);

        elem->bounds = imageLayer.bounds = Bounds(320, 240);
        imageLayer.hasBackground = true;
        imageLayer.backgroundColor = GFX::Color(0,0,0);
        imageLayer.add(elem);
        addLayer(&imageLayer);
    }
};

template <>
Activity *Activity::Create<ImageViewActivity>(const FS::Path &path) {
	return new ImageViewActivity(path);
}
