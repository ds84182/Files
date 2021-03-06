#include "activity.hpp"
#include "manager.hpp"

#include <animation/controller.hpp>

#include <ui/elements/directory_entry.hpp>
#include <ui/layouts/recycler_layout.hpp>

#include <style/color.hpp>

using UI::Elements::DirectoryEntryData;
using UI::Elements::DirectoryEntryElement;

class FileBrowserActivity : public Activity, public UI::Layouts::RecyclerLayout::Adapter {
public:
	FileBrowserActivity() : Activity(), directoryList(this) {init();}
	FileBrowserActivity(const FS::Path &path) :
		Activity(), path(path), directoryList(this) {init();}

	// Called when the Activity is requested to start
	virtual void onStart() override;
	virtual void onUpdate(float delta) override;
	virtual void onFinish() override;
	virtual void onKeyReleased(u32 keys) override;

	virtual UI::ElementBase *createElement() override {
		return new DirectoryEntryElement();
	}

	virtual void bindElement(UI::ElementBase *element, unsigned int position) override {
		DirectoryEntryElement *delem = reinterpret_cast<DirectoryEntryElement*>(element);
		delem->setData(data.at(position));
	}

	virtual unsigned int count() override {
		return data.size();
	}

private:
	FS::Path path;
	UI::Layouts::RecyclerLayout directoryList;
	Animation::Controller transitionController;
	bool transitionFinished = false;
	std::vector<DirectoryEntryData> data;

	void init();
	void loadEntries();
};

void FileBrowserActivity::init() {
	directoryList.bounds = Bounds(320, 240); // Use the entire screen
	directoryList.elementSize = 48;

	directoryList.onSelected = [=](int index) {
		const DirectoryEntryData &data = this->data.at(index);
		printf("File selected: %s (index %d)\n", data.path.str().c_str(), index);

		if (data.type == FS::EntryType::Directory) {
			printf("Enter directory\n");

			ActivityManager::Start<FileBrowserActivity>(data.path);
		} else if (data.type == FS::EntryType::File) {
			ActivityManager::Start<ImageViewActivity>(data.path);
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
	data.clear();
	while (true) {
		FS::DirectoryEntry entry = dir.next();
		if (entry.type == FS::EntryType::None) break;
		data.emplace_back(entry);
	}

	// Sort the entries
	std::sort(data.begin(), data.end(), [](auto &a, auto &b) {
		if (a.type == FS::EntryType::Directory && b.type != a.type) return true;
		if (b.type == FS::EntryType::Directory && a.type != b.type) return false;
		return strcasecmp(a.path.name().c_str(), b.path.name().c_str()) < 0;
	});

	notifyChanged();
}

void FileBrowserActivity::onUpdate(float delta) {
	transitionController.update(delta);
	if (transitionFinished) {
		finish();
	}
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
			transitionFinished = true;
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

template <>
Activity *Activity::Create<FileBrowserActivity>() {
	return new FileBrowserActivity();
}

template <>
Activity *Activity::Create<FileBrowserActivity>(const FS::Path &path) {
	return new FileBrowserActivity(path);
}
