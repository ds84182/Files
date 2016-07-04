#pragma once

#include <string>

namespace UI {
namespace StatusBarManager {

void Init();
void Update(float delta);
void SetBlack(bool black);
void SetDebugText(std::wstring text);

}
}
