#include "directory_entry.h"

#include "graphics/circle.h"
#include "graphics/rectangle.h"
#include "graphics/scissor.h"

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
		int height = bounds.height();
		int textHeight = GFX::noto.height();
		// TODO: GFX::Color::Black
		GFX::noto.drawText(data.name, bounds.left+8, bounds.top+(height/2)+(textHeight/2), GFX::Color(0, 0, 0));
	}

	ripple.setCenter(bounds);
	ripple.render(timeDelta);
}
