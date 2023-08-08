#include "JSWrapper.h"

// TODO: Add comments

Game::Game(std::string collisionMeshesFolder, CarConfigPreset preset)
{
    RocketSim::Init(collisionMeshesFolder.c_str());

    arena = Arena::Create(GameMode::SOCCAR);

    carConfigPreset = preset;
    car = arena->AddCar(Team::BLUE, GetCarConfig());
}

Game::~Game()
{
    delete arena;
}

CarControls Game::GetControls()
{
    return car->controls;
}

void Game::SetControls(CarControls controls)
{
    car->controls = controls;
}

void Game::Step(int ticksToSimulate)
{
    CarState carState = car->GetState();
    carState.boost = 100;
    car->SetState(carState);
    arena->Step(ticksToSimulate);
}

GameState Game::GetState()
{
    GameState state = {};

    BallState ballState = arena->ball->GetState();
    state.ballPos = ballState.pos;
    state.ballVel = ballState.vel;
    state.ballAngVel = ballState.angVel;

    CarState carState = car->GetState();
    state.carPos = carState.pos;
    state.carRotMat = carState.rotMat;
    state.carAng = Angle::FromRotMat(carState.rotMat);
    state.carForward = state.carAng.GetForwardVector();
    state.carVel = carState.vel;
    state.carAngVel = carState.angVel;

    return state;
}

void Game::ResetToKickoff()
{
    arena->ResetToRandomKickoff();
}

CarConfig Game::GetCarConfig()
{
    switch (carConfigPreset)
    {
    case CarConfigPreset::OCTANE:
        return CAR_CONFIG_OCTANE;
    case CarConfigPreset::DOMINUS:
        return CAR_CONFIG_DOMINUS;
    case CarConfigPreset::PLANK:
        return CAR_CONFIG_PLANK;
    case CarConfigPreset::BREAKOUT:
        return CAR_CONFIG_BREAKOUT;
    case CarConfigPreset::HYBRID:
        return CAR_CONFIG_HYBRID;
    case CarConfigPreset::MERC:
        return CAR_CONFIG_MERC;
    }
}

#include "bind.h"
using namespace emscripten;

EMSCRIPTEN_BINDINGS(rocketsim)
{
    class_<Game>("Game")
        .constructor<std::string, CarConfigPreset>()
        .function("GetControls", &Game::GetControls)
        .function("SetControls", &Game::SetControls)
        .function("Step", &Game::Step)
        .function("GetState", &Game::GetState)
        .function("ResetToKickoff", &Game::ResetToKickoff)
        .function("GetCarConfig", &Game::GetCarConfig);

    value_object<GameState>("GameState")
        .field("ballPos", &GameState::ballPos)
        .field("ballVel", &GameState::ballVel)
        .field("ballAngVel", &GameState::ballAngVel)
        .field("carPos", &GameState::carPos)
        .field("carRotMat", &GameState::carRotMat)
        .field("carAng", &GameState::carAng)
        .field("carForward", &GameState::carForward)
        .field("carVel", &GameState::carVel)
        .field("carAngVel", &GameState::carAngVel);

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

    enum_<CarConfigPreset>("CarConfigPreset")
        .value("OCTANE", CarConfigPreset::OCTANE)
        .value("DOMINUS", CarConfigPreset::DOMINUS)
        .value("PLANK", CarConfigPreset::PLANK)
        .value("BREAKOUT", CarConfigPreset::BREAKOUT)
        .value("HYBRID", CarConfigPreset::HYBRID)
        .value("MERC", CarConfigPreset::MERC);

    constant("BALL_COLLISION_RADIUS_NORMAL", RLConst::BALL_COLLISION_RADIUS_NORMAL);
}