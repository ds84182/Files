#include "directory_entry.hpp"

#include <graphics/circle.hpp>
#include <graphics/rectangle.hpp>
#include <graphics/scissor.hpp>

using Animation::LogDecelerationInterpolatorFactory;
using Animation::ValueTarget;

void UI::Elements::DirectoryEntryElement::init() {
	this->onTouchStart = [this](int x, int y) {
		ripple.setBounds(bounds);
		ripple.onTouchStart(x, y);
		return true;
	};

	this->onTouchMove = [this](int x, int y) {
		ripple.setBounds(bounds);
		ripple.setHotspot(x, y);
		return true;
	};

	this->onTouchEnd = [this](int x, int y) {
		ripple.setBounds(bounds);
		ripple.onTouchEnd(x, y);
		return true;
	};
}

void UI::Elements::DirectoryEntryElement::render(float timeDelta) {
	GFX::Scissor scissor(bounds);

	if (data.valid) {
		auto &font = GFX::Fonts::Body1;
		int height = bounds.height();
		int textHeight = font.height();
		// TODO: GFX::Color::Black
		font.drawText(data.name, bounds.left+16, bounds.top+((height/2)-(textHeight/2)), GFX::Color(0, 0, 0));
	}

	ripple.setCenter(bounds);
	ripple.render(timeDelta);
}
