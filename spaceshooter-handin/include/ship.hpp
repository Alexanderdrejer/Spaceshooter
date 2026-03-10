#pragma once
#include "game_state.hpp"
#include "buttons.hpp"

class Ship {
public:
    Ship(GameState *game, Buttons *buttons);
    void moveShip(Vec2 move);
    void shipThread();

private:
    GameState *game;
    Buttons *buttons;
};