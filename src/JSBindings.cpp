#include "RocketSim.h"

#include "bind.h"
using namespace emscripten;

EMSCRIPTEN_BINDINGS(rocketsim)
{
    value_object<CarControls>("CarControls")
        .field("throttle", &CarControls::throttle)
        .field("steer", &CarControls::steer)
        .field("pitch", &CarControls::pitch)
        .field("yaw", &CarControls::yaw)
        .field("roll", &CarControls::roll)
        .field("boost", &CarControls::boost)
        .field("jump", &CarControls::jump)
        .field("handbrake", &CarControls::handbrake);

    value_array<Vec>("Vec")
        .element(&Vec::x)
        .element(&Vec::z)
        .element(&Vec::y);

    value_array<RotMat>("RotMat")
        .element(&RotMat::forward)
        .element(&RotMat::right)
        .element(&RotMat::up);

    value_array<Angle>("Angle")
        .element(&Angle::roll)
        .element(&Angle::yaw)
        .element(&Angle::pitch);

    value_array<std::array<Vec, 4>>("VecArray4")
        .element(emscripten::index<0>())
        .element(emscripten::index<1>())
        .element(emscripten::index<2>())
        .element(emscripten::index<3>());

    value_array<std::array<Angle, 4>>("AngleArray4")
        .element(emscripten::index<0>())
        .element(emscripten::index<1>())
        .element(emscripten::index<2>())
        .element(emscripten::index<3>());

    value_object<CarConfig>("CarConfig")
        .field("hitboxSize", &CarConfig::hitboxSize)
        .field("hitboxPosOffset", &CarConfig::hitboxPosOffset)
        .field("frontWheels", &CarConfig::frontWheels)
        .field("backWheels", &CarConfig::backWheels)
        .field("dodgeDeadzone", &CarConfig::dodgeDeadzone);

    value_object<WheelPairConfig>("WheelPairConfig")
        .field("wheelRadius", &WheelPairConfig::wheelRadius)
        .field("suspensionRestLength", &WheelPairConfig::suspensionRestLength)
        .field("connectionPointOffset", &WheelPairConfig::connectionPointOffset);

    constant("BALL_COLLISION_RADIUS_SOCCAR", RLConst::BALL_COLLISION_RADIUS_SOCCAR);
}