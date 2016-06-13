// Standard C
//#include <cstring>

// C++ STL
//#include <codecvt>
//#include <locale>

// 3DS Libraries
#include <3ds.h>
#include <citro3d.h>

// Activity Subsystem
#include "activity/manager.h"
#include "activity/file_browser.h"

// Graphics Subsystem
#include "graphics/core.h"
//#include "graphics/rectangle.h"
//#include "graphics/fonts.h"

// UI Subsystem
#include "ui/manager.h"
//#include "ui/elements/directory_entry.h"
//#include "ui/layouts/recycler_layout.h"

// Utilities
//#include "utils/bounds.h"
#include "utils/filesystem/path.h"
//#include "utils/filesystem/directory.h"

#define TICKS_PER_SEC (268123480)

int main() {
	GFX::Init();

	/*GFX::RectangleNode rect(0, 0, 320, 64, GFX::Color(255, 0, 0));
	GFX::RectangleNode rect2(0, 64, 320, 64, GFX::Color(0, 255, 0));*/

	consoleInit(GFX_TOP, NULL);

	/*ssize_t nameLength = utf8_to_utf32(nullptr, (const u8*) u8"Unicode: ( ͡° ͜ʖ ͡°)", 0);
	std::u32string ustr(nameLength, '\0');
	utf8_to_utf32((u32*) ustr.data(), (const u8*) u8"Unicode: ( ͡° ͜ʖ ͡°)", ustr.length());

	printf("ustrlen = %d %d\n", ustr.length(), sizeof(wchar_t));*/

	/*FS::Path testPath("/hello/world");

	printf("path part count = %d\n", testPath.parts.size());
	for (const std::string &str : testPath.parts) {
		std::cout << str << " ";
	}
	std::cout << std::endl;
	std::cout << testPath << std::endl;*/

	sdmcInit();
	FS::Path sdRoot("sdmc:/");
	//FS::Directory root(sdRoot);
	/*std::cout << sdRoot << ": " << root.count() << std::endl;

	while (true) {
		FS::DirectoryEntry entry = root.next();
		if (entry.type == FS::EntryType::None) break;
		std::cout << entry.path() << std::endl;
	}*/

	ActivityManager::Start<FileBrowserActivity>(sdRoot);

	/*using namespace UI::Elements;
	DirectoryEntryElement entryElement(DirectoryEntryData(root.next()));
	entryElement.bounds = Bounds(0, 0, 320, 64);
	DirectoryEntryElement entryElement2(DirectoryEntryData(root.next()));
	entryElement2.bounds = Bounds(0, 64, 320, 64);*/

	/*
	Layout and UI thread communication example:
	RecyclerLayout<DirectoryEntryElement, DirectoryEntryData> directoryList();
	directoryList.bounds = UI::Bounds(320, 240); // Use the entire screen

	directoryList.onClick = [&](DirectoryEntryData &data, int x, int y) {
		// Called when an element is clicked.
		// Notice that there is no way to get the directory element.
		// This is done so people won't be tempted to access the element in a thread unsafe maner.
	};

	while (true) {
		FS::DirectoryEntry entry = root.next();
		if (entry.type == FS::EntryType::None) break;
		directoryList.emplace_back(entry); // directoryList also extends a std::vector of data
	}

	directoryList.update(); // Update needs to be called once so that the elements are put on screen.
	*/

	/*using namespace UI::Elements;
	using namespace UI::Layouts;

	RecyclerLayout<DirectoryEntryElement> directoryList;
	directoryList.bounds = Bounds(320, 240); // Use the entire screen
	directoryList.elementSize = 64;
	directoryList.onSelected = [&](DirectoryEntryData &data, int index) {
		printf("File selected: %s (index %d)\n", data.path.str().c_str(), index);

		if (data.type == FS::EntryType::Directory) {
			printf("Enter directory\n");
			FS::Directory dir(data.path);
			directoryList.clear();

			while (true) {
				FS::DirectoryEntry entry = dir.next();
				if (entry.type == FS::EntryType::None) break;
				directoryList.data.emplace_back(entry);
			}
			std::sort(directoryList.data.begin(), directoryList.data.end(), [](auto &a, auto &b) {
				return a.path.name() < b.path.name();
			});

			directoryList.update(); // Update needs to be called so that the elements are put on screen.
		}
	};

	while (true) {
		FS::DirectoryEntry entry = root.next();
		if (entry.type == FS::EntryType::None) break;
		directoryList.data.emplace_back(entry);
	}
	std::sort(directoryList.data.begin(), directoryList.data.end(), [](auto &a, auto &b) {
		return a.path.name() < b.path.name();
	});

	directoryList.update(); // Update needs to be called so that the elements are put on screen.

	int scrollVelocity = 0;*/
	u64 lastTick = svcGetSystemTick();

	// Main loop
	while (aptMainLoop()) {
		u64 newTick = svcGetSystemTick();
		float timeDelta = ((float)(newTick-lastTick))/TICKS_PER_SEC;
		lastTick = newTick;
		//printf("Frame time %f\n", timeDelta*1000.0f);

		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		UI::Manager::UpdateTemporaryFunctionToUpdateTheUIBecauseUIThreadDoesntExistYet(timeDelta);

		/*if (kHeld & KEY_DOWN) {
			scrollVelocity+=2;
		} else if (kHeld & KEY_UP) {
			scrollVelocity-=2;
		}

		if (scrollVelocity) {
			directoryList.onScroll(scrollVelocity);

			if (scrollVelocity > 0) {
				scrollVelocity--;
			} else {
				scrollVelocity++;
			}
		}*/

		/*if (kDown & KEY_TOUCH) {
			touchPosition touch;
			hidTouchRead(&touch);
			if (entryElement.bounds.inside(touch.px, touch.py)) {
				entryElement.bounds.top += 10;
				entryElement.bounds.bottom += 10;
				rect.y += 10;
			}
		}*/

		/*GFX::FrameStart(GFX::Screen::Top);

			GFX::SetFragMode(GFX::Replace);
			rect.render();

		GFX::FrameEnd();*/

		GFX::FrameBegin();

			UI::Manager::Compost(timeDelta);

			GFX::DrawOn(GFX::Screen::Bottom);

			//GFX::SetFragMode(GFX::Replace);
			//rect.render();
			//rect2.render();


			//GFX::roboto.drawText(U"3ds", 0, 16, GFX::Color(0, 0, 0));
			//GFX::roboto.drawText(U"Nintendo 3DS", 0, 240, GFX::Color(0, 0, 0));
			//entryElement.render();
			//entryElement2.render();
			UI::Manager::Render(timeDelta);
			//GFX::noto.drawText(ustr, 0, 64, GFX::Color(0, 0, 0));

		GFX::FrameEnd();
	}

	GFX::Deinit();
	return 0;
}
