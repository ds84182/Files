#pragma once

#include <deque>
#include <memory>
#include <utility>

#include "activity.h"

namespace ActivityManager {

// TODO: This probably needs a lock!
extern Activity *current;
extern std::deque<Activity*> stack;

// TODO: Enter and Finish need to send a callback to be called in the activity (depends on lock)
// This is needed because the Activity may only take requests as a suggestion
// For instance, requestFinish could prompt the Activity to play an animation before exiting fully
inline void Enter(Activity *activity) {
	stack.emplace_back(current);
	current = activity;
	current->dispatchStart();
}

template <class T, class... Args>
void Start(Args&&... args) {
	std::shared_ptr<Handler> handler = std::make_shared<Handler>();
	new HandlerThread(handler); // HandlerThread cleans itself up
	Activity *activity = new T(handler, std::forward<Args>(args)...);
	Enter(activity);
}

inline void Finish() {
	current->dispatchFinish();
	if (stack.empty()) {
		current = nullptr; // TODO: Exit
	} else {
		current = stack.back();
		stack.pop_back();
	}
}

}
