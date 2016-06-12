#include "file_browser.h"
#include "manager.h"

using UI::Elements::DirectoryEntryData;

void FileBrowserActivity::init() {
	directoryList.bounds = Bounds(320, 240); // Use the entire screen
	directoryList.elementSize = 64;

	directoryList.onSelected = [=](DirectoryEntryData &data, int index) {
		printf("File selected: %s (index %d)\n", data.path.str().c_str(), index);

		if (data.type == FS::EntryType::Directory) {
			printf("Enter directory\n");

			FS::Path &dirpath = data.path;
			handler->postCallbackDelayed([=]() {
				this->path = dirpath;
				loadEntries();
			}, 500);
		}
	};

	addLayout(directoryList);
}

void FileBrowserActivity::onStart() {
	loadEntries();
}

void FileBrowserActivity::loadEntries() {
	FS::Directory dir(path);

	// Load the entries
	directoryList.clear();
	while (true) {
		FS::DirectoryEntry entry = dir.next();
		if (entry.type == FS::EntryType::None) break;
		directoryList.data.emplace_back(entry);
	}

	// Sort the entries
	std::sort(directoryList.data.begin(), directoryList.data.end(), [](auto &a, auto &b) {
		return a.path.name() < b.path.name();
	});

	directoryList.update(); // Update needs to be called so that the elements are put on screen.
}

void FileBrowserActivity::onKeyReleased(u32 keys) {
	if (keys & KEY_B) {
		path = path.parent();
		loadEntries();
	}
}
