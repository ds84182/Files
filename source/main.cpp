// 3DS Libraries
#include <3ds.h>
#include <citro3d.h>

// Activity Subsystem
#include "activity/manager.h"
#include "activity/file_browser.h"

// Graphics Subsystem
#include "graphics/core.h"
#include "graphics/rectangle.h"

// UI Subsystem
#include "ui/manager.h"

// Utilities
#include "utils/filesystem/path.h"
#include "utils/math_ext.h"

#define TICKS_PER_SEC (268123480)

C3D_Mtx projection;

int main() {
	GFX::Init();

	consoleInit(GFX_TOP, NULL);

	sdmcInit();
	FS::Path sdRoot("sdmc:/");

	ActivityManager::Start<FileBrowserActivity>(sdRoot);

	u64 lastTick = svcGetSystemTick();

	// Main loop
	while (aptMainLoop()) {
		u64 newTick = svcGetSystemTick();
		float timeDelta = ((float)(newTick-lastTick))/TICKS_PER_SEC;
		lastTick = newTick;

		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		UI::Manager::Update(timeDelta);

		GFX::FrameBegin();

			UI::Manager::Compost(timeDelta);

			GFX::DrawOn(GFX::Screen::Bottom);
			UI::Manager::Render(timeDelta);

		GFX::FrameEnd();
	}

	GFX::Deinit();
	return 0;
}
