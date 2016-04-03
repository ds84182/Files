#pragma once

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "target.h"
#include "time_interpolator.h"

namespace Animation {

enum class State : unsigned char {
	Running,
	Paused,
	Done
};

struct AnimationState {
	TimeInterpolator interpolator;
	Target target;
	float time;
	float duration;
	State state;
	bool reverse;
	AnimationState(TimeInterpolator interpolator, Target target, float duration,
			float time = 0.0, State state = State::Running, bool reverse = false) :
		interpolator(interpolator), target(target), time(time), duration(duration), state(state), reverse(reverse) {}
};

// Controls the interactions between time interpolators and targets.
// Also does state management.
class Controller {
public:
	void start(TimeInterpolator interpolator, Target target, float duration,
			float time = 0.0, std::string tag = "", State state = State::Running, bool reverse = false) {
		animations.emplace_back(interpolator, target, duration, time, state, reverse);
		if (!tag.empty()) {
			tagMap.emplace(tag, &animations.back());
		}
	}
	
	void clear() {
		animations.clear();
	}
	
	bool hasTag(std::string tag) {
		return tagMap.count(tag) > 0;
	}
	
	AnimationState *getTag(std::string tag) {
		return hasTag(tag) ? tagMap.at(tag) : nullptr;
	}
	
	void update(float delta) {
		// TODO: std::remove_if done!
		for (auto &state : animations) {
			if (state.state == State::Running) {
				state.time += state.reverse ? -delta : delta;
			
				// TODO: onDone callbacks?
				if ((!state.reverse) && state.time > state.duration) {
					state.time = state.duration;
					state.state = State::Done;
				} else if (state.reverse && state.time < 0.0) {
					state.time = 0.0;
					state.state = State::Done;
				}
			
				state.target(state.interpolator(state.time/state.duration));
			}
		}
	}
private:
	std::vector<AnimationState> animations;
	std::map<std::string, AnimationState*> tagMap;
};

}
