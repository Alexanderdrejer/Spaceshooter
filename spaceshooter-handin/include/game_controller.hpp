#pragma once
#include "game_state.hpp"

class GameController {
public:
    explicit GameController(GameState *game);

    void gameControllerThread();

private:
    GameState *game;
};