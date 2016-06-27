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

			//FS::Path &dirpath = data.path;
			/*handler->postCallbackDelayed([=]() {
				this->path = dirpath;
				loadEntries();
			}, 500);*/
			ActivityManager::Start<FileBrowserActivity>(data.path);
		}
	};

	addLayout(directoryList);
}

void FileBrowserActivity::onStart() {
	loadEntries();

	directoryList.elementLayer.startCompost(true);
	directoryList.elementLayer.hasBackground = true;
	directoryList.elementLayer.backgroundColor = GFX::Color(255,255,255);

	transitionController.start(
		&Animation::LinearOutSlowInInterpolator,
		Animation::ValueTarget<GFX::Color>(
			&directoryList.elementLayer.compostColor,
			GFX::Color(255, 255, 255, 0),
			GFX::Color(255, 255, 255, 255)),
		0.270f, [&](Animation::Controller *controller, Animation::AnimationState &state) {
			ActivityManager::HidePrevious();
		});
	transitionController.start(
		&Animation::LinearOutSlowInInterpolator,
		Animation::ValueTarget<float>(
			&directoryList.elementLayer.y,
			240*0.16,
			0),
		0.270f);
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

void FileBrowserActivity::onFinish() {
	ActivityManager::ShowPrevious();
	transitionController.start(
		&Animation::LinearOutSlowInInterpolator,
		Animation::ValueTarget<GFX::Color>(
			&directoryList.elementLayer.compostColor,
			GFX::Color(255, 255, 255, 255),
			GFX::Color(255, 255, 255, 0)),
		0.270f, [&](Animation::Controller *controller, Animation::AnimationState &state) {
			finish();
		});
	transitionController.start(
		&Animation::LinearOutSlowInInterpolator,
		Animation::ValueTarget<float>(
			&directoryList.elementLayer.y,
			0,
			240*0.16),
		0.270f);
}

void FileBrowserActivity::onKeyReleased(u32 keys) {
	if (keys & KEY_B) {
		onFinish();
	}
}
