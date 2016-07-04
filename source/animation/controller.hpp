#pragma once

#include <algorithm>
#include <cstdlib>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "target.hpp"
#include "time_interpolator.hpp"

namespace Animation {

enum class State : unsigned char {
	Running,
	Paused,
	Done
};

using OnDone = std::function<void(class Controller*, struct AnimationState&)>;

struct AnimationState {
	TimeInterpolator interpolator;
	Target target;
	float time;
	float duration;
	State state;
	bool reverse;
	OnDone onDone;
	std::string tag;
	AnimationState(TimeInterpolator interpolator, Target target, float duration,
			OnDone onDone = nullptr, std::string tag = "", float time = 0.0,
			State state = State::Running, bool reverse = false) :
		interpolator(interpolator), target(target), time(time), duration(duration), state(state),
		reverse(reverse), onDone(onDone), tag(tag) {}
};

// Controls the interactions between time interpolators and targets.
// Also does state management.
class Controller {
public:
	AnimationState &start(TimeInterpolator interpolator, Target target, float duration,
			OnDone onDone = nullptr, std::string tag = "", float time = 0.0, State state = State::Running,
			bool reverse = false) {
		animations.emplace_back(interpolator, target, duration, onDone, tag, time, state, reverse);
		AnimationState &anim = animations.back();
		if (!tag.empty()) {
			tagMap.emplace(tag, anim);
		}
		if (state == State::Running)
			anim.target(anim.interpolator(anim.time/anim.duration));
		return anim;
	}

	void clear() {
		animations.clear();
	}

	bool hasTag(std::string tag) {
		return tagMap.count(tag) > 0;
	}

	AnimationState &getTag(std::string tag) {
		if (!hasTag(tag)) abort();
		return tagMap.at(tag);
	}

	void cancelTag(std::string tag) {
		if (!hasTag(tag)) abort();
		animations.erase(std::remove_if(animations.begin(), animations.end(), [&](auto &state) {
			return state.tag == tag;
		}));
	}

	void update(float delta) {
		// TODO: std::remove_if done!
		for (auto &state : animations) {
			if (state.state == State::Running) {
				state.time += state.reverse ? -delta : delta;

				if ((!state.reverse) && state.time > state.duration) {
					state.time = state.duration;
					state.state = State::Done;
				} else if (state.reverse && state.time < 0.0) {
					state.time = 0.0;
					state.state = State::Done;
				}

				state.target(state.interpolator(state.time/state.duration));

				if (state.state == State::Done) {
					if (state.onDone) {
						state.onDone(this, state);
					}
				}
			}
		}

		if (!animations.empty())
			animations.erase(std::remove_if(animations.begin(), animations.end(), [](auto &state) {
				return state.state == State::Done;
			}), animations.end());
	}
private:
	std::vector<AnimationState> animations;
	std::map<std::string, AnimationState&> tagMap;
};

}
