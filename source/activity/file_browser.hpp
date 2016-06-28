#pragma once

#include "activity.hpp"

#include <animation/controller.hpp>

#include <ui/layouts/recycler_layout.hpp>
#include <ui/elements/directory_entry.hpp>

class FileBrowserActivity : public Activity {
public:
	// Every activity has its own thread with a handler. Events are delivered to the activity through the handler.
	FileBrowserActivity() : Activity() {init();}
	FileBrowserActivity(const FS::Path &path) :
		Activity(), path(path) {init();}

	// Called when the Activity is requested to start
	virtual void onStart() override;
	virtual void onUpdate(float delta) override;
	virtual void onFinish() override;
	virtual void onKeyReleased(u32 keys) override;

private:
	FS::Path path;
	UI::Layouts::RecyclerLayout<UI::Elements::DirectoryEntryElement> directoryList;
	UI::Layer topLayer;
	Animation::Controller transitionController;

	void init();
	void loadEntries();
};
