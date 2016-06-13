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

	directoryList.update();
	// This updates because the layout bounds need to be set before compost starts

	addLayout(directoryList);

	directoryList.elementLayer.startCompost(true);

	transitionController.start(
		&Animation::LinearOutSlowInInterpolator,
		Animation::ValueTarget<GFX::Color>(
			&directoryList.elementLayer.compostColor,
			GFX::Color(255, 255, 255, 0),
			GFX::Color(255, 255, 255, 255)),
		0.270f);
	transitionController.start(
		&Animation::LinearOutSlowInInterpolator,
		Animation::ValueTarget<float>(
			&directoryList.elementLayer.y,
			240*0.16,
			0),
		0.270f);
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
		if (a.type == FS::EntryType::Directory && b.type != a.type) return true;
		if (b.type == FS::EntryType::Directory && a.type != b.type) return false;
		return strcasecmp(a.path.name().c_str(), b.path.name().c_str()) < 0;
	});

	directoryList.update(); // Update needs to be called so that the elements are put on screen.
}

void FileBrowserActivity::onUpdate(float delta) {
	transitionController.update(delta);
}

void FileBrowserActivity::onKeyReleased(u32 keys) {
	if (keys & KEY_B) {
		path = path.parent();
		loadEntries();
	}
}
