#pragma once
#include "game_state.hpp"

class Enemy {
public:
    Enemy(GameState *game);
    void enemyThread();

private:
    GameState *game;
};