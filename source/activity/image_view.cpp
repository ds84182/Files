#include "activity.hpp"
#include "manager.hpp"

#include <graphics/texture.hpp>

#include <fmt/format.hpp>

#include <ui/element.hpp>
#include <ui/status_bar_manager.hpp>

#include <utils/filesystem/path.hpp>
#include <utils/lodepng.hpp>

#include <style/color.hpp>

class ImageViewActivity : public Activity {
public:
    FS::Path path;
    GFX::Texture texture;
    UI::Layer imageLayer;
    ImageViewActivity(const FS::Path &path) : path(path) {}

    virtual void onStart() override {
        unsigned char *image;
        unsigned width, height;

        //decode
        unsigned error = lodepng_decode32_file(&image, &width, &height, path.str().c_str());

        texture.create(width, height, GPU_RGBA8);
        texture.copyAndTile(reinterpret_cast<u32*>(image), width, height);
        free(image);

        fmt::WMemoryWriter debugWriter;
        debugWriter << L"File: " << path.str() << L'\n';
        debugWriter << L"Image size: " << width << L'x' << height << L'\n';
        if (error) debugWriter << L"Error " << error << L": " << lodepng_error_text(error);
        UI::StatusBarManager::SetDebugText(debugWriter.str());

        UI::StatusBarManager::SetBlack(true);

        imageLayer.bounds = Bounds(320, 240);
        imageLayer.hasBackground = true;
        imageLayer.backgroundColor = GFX::Color(255,255,255);
        imageLayer.backgroundTexture = &texture;
        addLayer(&imageLayer);
    }
};

template <>
Activity *Activity::Create<ImageViewActivity>(const FS::Path &path) {
	return new ImageViewActivity(path);
}
