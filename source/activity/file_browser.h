#pragma once

#include "activity.h"

#include <animation/controller.h>

#include <ui/layouts/recycler_layout.h>
#include <ui/elements/directory_entry.h>

class FileBrowserActivity : public Activity {
public:
	// Every activity has its own thread with a handler. Events are delivered to the activity through the handler.
	FileBrowserActivity(std::shared_ptr<Handler> handler) : Activity(handler) {init();}
	FileBrowserActivity(std::shared_ptr<Handler> handler, const FS::Path &path) :
		Activity(handler), path(path) {init();}

	// Called when the Activity is requested to start
	// TODO: Load directory stuff on worker thread, push to this activity's thread
	virtual void onStart() override;
	virtual void onUpdate(float delta) override;
	virtual void onKeyReleased(u32 keys) override;

private:
	FS::Path path;
	UI::Layouts::RecyclerLayout<UI::Elements::DirectoryEntryElement> directoryList;
	Animation::Controller transitionController;

	void init();
	void loadEntries();
};
