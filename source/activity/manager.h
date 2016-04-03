#pragma once

#include <deque>
#include <memory>
#include <utility>

#include "activity.h"

namespace ActivityManager {

// TODO: This probably needs a lock!
extern std::unique_ptr<Activity> current;
extern std::deque<std::unique_ptr<Activity>> stack;

// TODO: Enter and Finish need to send a callback to be called in the activity (depends on lock)
inline void Enter(std::unique_ptr<Activity> activity) {
	stack.emplace_back(std::move(current));
	current = std::move(activity);
	current->requestStart();
}

template <class T, class... Args>
void Start(Args&&... args) {
	std::shared_ptr<Handler> handler = std::make_shared<Handler>();
	new HandlerThread(handler); // HandlerThread cleans itself up
	auto activity = std::unique_ptr<Activity>(new T(handler, std::forward<Args>(args)...));
	Enter(std::move(activity));
}

inline void Finish() {
	current->requestFinish();
	if (stack.empty()) {
		current.reset();
	} else {
		current = std::move(stack.back());
		stack.pop_back();
	}
}

}
