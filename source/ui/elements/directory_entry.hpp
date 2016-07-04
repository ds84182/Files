#pragma once

#include <3ds.h>

#include <graphics/fonts.hpp>
#include <graphics/ripple.hpp>

#include <ui/element.hpp>

#include <utils/filesystem/directory.hpp>
#include <utils/filesystem/path.hpp>

namespace UI {
namespace Elements {

struct DirectoryEntryData {
	bool valid = false;
	FS::Path path;
	FS::EntryType type = FS::EntryType::None;
	std::wstring name;

	DirectoryEntryData() {}

	DirectoryEntryData(const FS::Path path, const FS::EntryType type) :
		valid(true), path(path), type(type) {convertName();};

	DirectoryEntryData(const FS::DirectoryEntry &entry) :
		valid(true), path(entry.path()), type(entry.type) {convertName();};

private:
	void convertName() {
		if (name.empty()) {
			// Convert the given utf8 string into utf32
			ssize_t nameLength = utf8_to_utf32(nullptr, (const u8*)path.name().c_str(), 0);
			name = std::wstring(nameLength, '\0');
			utf8_to_utf32((u32*)name.data(), (const u8*)path.name().c_str(), name.length());
		}
	}
};

class DirectoryEntryElement : public Element<DirectoryEntryData> {
public:
	DirectoryEntryElement() {init();}
	DirectoryEntryElement(const DirectoryEntryData &data) : Element(data) {init();}

	void init();
	virtual void render(float timeDelta) override;
private:
	GFX::Ripple ripple;
};

}
}
