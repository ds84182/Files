#pragma once

#include <deque>
#include <memory>
#include <utility>

#include "activity.hpp"

namespace ActivityManager {

// TODO: This probably needs a lock!
extern Activity *current;
extern std::deque<Activity*> stack;

// TODO: Enter and Finish need to send a callback to be called in the activity (depends on lock)
// This is needed because the Activity may only take requests as a suggestion
// For instance, requestFinish could prompt the Activity to play an animation before exiting fully
inline void Enter(Activity *activity) {
	if (current) {
		stack.emplace_back(current);
	}
	current = activity;
	current->dispatchStart();
}

template <class T, class... Args>
void Start(Args&&... args) {
	Activity *activity = new T(std::forward<Args>(args)...);
	Enter(activity);
}

inline void HidePrevious() {
	if (!stack.empty()) {
		stack.back()->hideLayers();
	}
}

inline void ShowPrevious() {
	if (!stack.empty()) {
		stack.back()->showLayers();
	}
}

inline void Finish() {
	current->dispatchFinish();
}

inline void FinishNow() {
	if (stack.empty()) {
		current = nullptr; // TODO: Exit
	} else {
		current = stack.back();
		stack.pop_back();
		current->showLayers();
	}
}

}