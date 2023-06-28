#include "RocketSim.h"

struct GameState
{
    Vec carPos;
    Angle carAng;
    Vec carVel;
    Vec carAngVel;

    Vec ballPos;
    Angle ballAng;
    Vec ballVel;
    Vec ballAngVel;

    Vec wheelPos[4];
    Angle wheelAng[4];
};

enum class CarConfigPreset : byte
{
    OCTANE,
    DOMINUS,
    PLANK,
    BREAKOUT,
    HYBRID,
    MERC,
};

class Game
{
public:
    Game(std::string collisionMeshesFolder, CarConfigPreset carConfigPreset);

    ~Game();

    Arena *arena;

    Car *car;

    CarConfigPreset carConfigPreset;

    CarControls GetControls();

    void SetControls(CarControls controls);

    void Step(int ticksToSimulate);

    GameState GetState();

    void ResetToKickoff();

    CarConfig GetCarConfig();
};