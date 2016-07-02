#include "edge_effect.hpp"

#include <cmath>

#include <graphics/circle.hpp>

namespace UI {
namespace Elements {

#define TICKS_PER_MSEC 268123.480

const int RecedeTime = 600;
const int PullTime = 167;
const int PullDecayTime = 2000;

const float MaxAlpha = 0.5f;
const float MaxGlowScale = 2.0f;
const float PullGlowBegin = 0.0f;

const int MinVelocity = 100;
const int MaxVelocity = 10000;

const float Epsilon = 0.001f;

const float Angle = 0.5235987755983f; // math.pi/6;
const float Sin = 0.5f; // sin(Angle);
const float Cos = 0.86602540378444f; // cos(Angle);

const float PullDistanceAlphaGlowFactor = 0.8f;
const int VelocityGlowFactor = 6;

EdgeEffect::EdgeEffect() {
    interpolator = Animation::DecelerateInterpolatorFactory();
}
EdgeEffect::~EdgeEffect() {}

void EdgeEffect::setSize(int width, int height) {
    const float r = width * 0.75f / Sin;
    const float y = Cos * r;
    const float h = r - y;
    const float _or = height * 0.75f / Sin;
    const float oy = Cos * _or;
    const float oh = _or - oy;

    radius = r;
    baseGlowScale = h > 0 ? std::min(oh / h, 1.f) : 1.f;

    bounds = Bounds(bounds.left, bounds.top, width, std::min(height, (int)h));
}

#define getTimeMS() (svcGetSystemTick()/TICKS_PER_MSEC)

void EdgeEffect::onPull(float deltaDistance, float displacement) {
    const u64 now = getTimeMS();
    targetDisplacement = displacement;
    if (state == State::PullDecay && now-startTime < duration) {
        return;
    }
    if (state != State::Pull) {
        glowScaleY = std::max(PullGlowBegin, glowScaleY);
    }
    state = State::Pull;

    startTime = now;
    duration = PullTime;

    pullDistance += deltaDistance;

    const float absdd = std::abs(deltaDistance);
    glowAlpha = glowAlphaStart = std::min(MaxAlpha, glowAlpha+(absdd*PullDistanceAlphaGlowFactor));

    if (pullDistance == 0) {
        glowScaleY = glowScaleYStart = 0;
    } else {
        const float scale = std::max(0.f, 1.0f-1.0f/std::sqrt(std::abs(pullDistance) * bounds.height())-0.3f)/0.7f;
        glowScaleY = glowScaleYStart = scale;
    }

    glowAlphaFinish = glowAlpha;
    glowScaleYFinish = glowScaleY;
}

void EdgeEffect::onRelease() {
    pullDistance = 0;

    if (state != State::Pull && state != State::PullDecay) {
        return;
    }

    state = State::Recede;
    glowAlphaStart = glowAlpha;
    glowScaleYStart = glowScaleY;

    glowAlphaFinish = 0.0f;
    glowScaleYFinish = 0.0f;

    startTime = getTimeMS();
    duration = RecedeTime;
}

void EdgeEffect::onAbsorb(int velocity) {
    state = State::Absorb;
    velocity = std::min(std::max(MinVelocity, (int)std::abs(velocity)), MaxVelocity);

    startTime = getTimeMS();
    duration = 0.15f + (velocity * 0.02f);

    glowAlphaStart = 0.3f;
    glowScaleYStart = std::max(glowScaleY, 0.0f);

    glowScaleYFinish = std::min(0.025f + (velocity * (velocity / 100) * 0.00015f) / 2, 1.f);
    glowAlphaFinish = std::max(glowAlphaStart, std::min(velocity * VelocityGlowFactor * .00001f, MaxAlpha));
    targetDisplacement = 0.5f;
}

void EdgeEffect::draw(int x, int y, float rotation, int pivotX, int pivotY) {
    update();

    C3D_Mtx* mtx = GFX::PushMatrix();

    const float centerX = (bounds.left+bounds.right)/2.f;
    const float centerY = bounds.height()-radius;

	Mtx_Translate(mtx, x-pivotX, y-pivotY, 0);
    Mtx_RotateZ(mtx, rotation, true);
    Mtx_Translate(mtx, pivotX, pivotY, 0);
	Mtx_Scale(mtx, 1, std::min(glowScaleY, 1.f) * baseGlowScale, 1);
    Mtx_Translate(mtx, 0, 0, 0);

    const float displacement = std::max(0.f, std::min(this->displacement, 1.f)) - 0.5f;
    float translateX = bounds.width() * displacement / 2.f;

    Mtx_Translate(mtx, translateX, 0, 0);

    GFX::UpdateMatrix();

    GFX::Circle circle(centerX, centerY, radius, GFX::Color(0,0,0,0xff*glowAlpha));
    circle.render();

    GFX::PopMatrix();
}

void EdgeEffect::update() {
    const u64 time = getTimeMS();
    const float t = std::min((time - startTime) / duration, 1.f);

    const float interp = interpolator(t);

    glowAlpha = glowAlphaStart + (glowAlphaFinish - glowAlphaStart) * interp;
    glowScaleY = glowScaleYStart + (glowScaleYFinish - glowScaleYStart) * interp;
    displacement = (displacement + targetDisplacement) / 2;

    if (t >= 1.f-Epsilon) {
        switch (state) {
            case State::Absorb:
                state = State::Recede;
                startTime = time;
                duration = RecedeTime;

                glowAlphaStart = glowAlpha;
                glowScaleYStart = glowScaleY;

                glowAlphaFinish = glowScaleYFinish = 0.0f;
                break;
            case State::Pull:
                state = State::PullDecay;
                startTime = time;
                duration = PullDecayTime;

                glowAlphaStart = glowAlpha;
                glowScaleYStart = glowScaleY;

                glowAlphaFinish = glowScaleYFinish = 0.0f;
                break;
            case State::PullDecay:
                state = State::Recede;
                break;
            case State::Recede:
                state = State::Idle;
                break;
        }
    }
}

}
}
