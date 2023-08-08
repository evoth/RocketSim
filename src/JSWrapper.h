#include "RocketSim.h"

struct GameState
{
    Vec carPos;
    RotMat carRotMat;
    Angle carAng;
    Vec carForward;
    Vec carVel;
    Vec carAngVel;

    Vec ballPos;
    Vec ballVel;
    Vec ballAngVel;
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