// 3DS Libraries
#include <3ds.h>
#include <citro3d.h>

// Activity Subsystem
#include <activity/manager.hpp>

// Graphics Subsystem
#include <graphics/core.hpp>
#include <graphics/rectangle.hpp>

// UI Subsystem
#include <ui/manager.hpp>

// Utilities
#include <utils/filesystem/path.hpp>
#include <utils/math_ext.hpp>

#define TICKS_PER_SEC (268123480)

C3D_Mtx projection;

int main() {
	romfsInit();
	GFX::Init();

	//consoleInit(GFX_TOP, NULL);

	sdmcInit();
	FS::Path sdRoot("sdmc:/");

	ActivityManager::Start<FileBrowserActivity>((const FS::Path&)sdRoot);

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

			GFX::DrawOn(GFX::Screen::TopLeft);
			UI::Manager::Render(timeDelta);

			GFX::DrawOn(GFX::Screen::Bottom);
			UI::Manager::Render(timeDelta);

		GFX::FrameEnd();
	}

	GFX::Deinit();
	return 0;
}
