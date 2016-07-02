#pragma once

// EdgeEffect is a small class that allows scrolling containers to show a pretty edge effect
// Usually ScrollListener will manage these
// Adapted from Android Marshmallow's EdgeEffect class

#include <animation/time_interpolator.hpp>

#include <graphics/core.hpp>

#include <utils/bounds.hpp>

namespace UI {
namespace Elements {

class EdgeEffect {
public:
    EdgeEffect();
    ~EdgeEffect();

    enum class State {
        Idle, Pull, Absorb, Recede, PullDecay
    };

    // Sets the effect size in pixels
    void setSize(int width, int height);
    bool isFinished() {
        return state == State::Idle;
    }
    void finish() {
        state = State::Idle;
    }
    // Change in distance since last call (going away from edge), 0 - 1 (no change - full length)
    // Negative values go towards the edge
    // Displacement is the finger position on the opposite axis ranging from 0 - 1
    void onPull(float deltaDistance, float displacement);
    void onRelease();
    // When the scroll hits a boundary and cannot continue the EdgeEffect absorbs the velocity
    // Velocity in pixels per second
    void onAbsorb(int velocity);
    void draw(int x = 0, int y = 0, float rotation = 0, int pivotX = 0, int pivotY = 0);
private:
    float glowAlpha, glowScaleY, glowAlphaStart, glowAlphaFinish, glowScaleYStart, glowScaleYFinish;

    u64 startTime;
    float duration;

    Animation::TimeInterpolator interpolator;

    State state = State::Idle;

    float pullDistance;

    Bounds bounds;
    float radius, baseGlowScale, displacement = 0.5f, targetDisplacement = 0.5f;

    void update();
};

}
}
