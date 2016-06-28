#include "manager.hpp"

namespace ActivityManager {

Activity *current = nullptr;
std::deque<Activity*> stack;

void Enter(Activity *activity) {
	if (current) {
		stack.emplace_back(current);
	}
	current = activity;
	current->dispatchStart();
}

void HidePrevious() {
	if (!stack.empty()) {
		stack.back()->hideLayers();
	}
}

void ShowPrevious() {
	if (!stack.empty()) {
		stack.back()->showLayers();
	}
}

void Finish() {
	if (current) {
		current->dispatchFinish();
	}
}

void FinishNow() {
	if (stack.empty()) {
		current = nullptr; // TODO: Exit
	} else {
		current = stack.back();
		stack.pop_back();
		current->showLayers();
	}
}

}
