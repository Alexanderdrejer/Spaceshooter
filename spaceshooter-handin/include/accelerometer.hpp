#pragma once
#include "game_types.hpp"
#include "game_state.hpp"

class Accelerometer {
public:
    Accelerometer(GameState *game);
    Vec2 readAccelerometer();

private:
    GameState *game;
};
