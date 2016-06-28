#pragma once

#include <deque>
#include <memory>
#include <utility>

#include "activity.hpp"

namespace ActivityManager {

extern Activity *current;
extern std::deque<Activity*> stack;

void Enter(Activity *activity);

template <class T, class... Args>
void Start(Args&&... args) {
	Enter(Activity::Create<T>(std::forward<Args>(args)...));
}

void HidePrevious();
void ShowPrevious();

void Finish();
void FinishNow();

}
