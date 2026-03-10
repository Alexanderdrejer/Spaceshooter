#pragma once
#include "game_types.hpp"
#include "game_state.hpp"
#include <linux/gpio.h>

#define BTN_A 27
#define BTN_B 22
#define BTN_UP 12
#define BTN_DOWN 16
#define BTN_LEFT 5
#define BTN_RIGHT 6

class Buttons {
public:
    Buttons(GameState *game);
    ~Buttons();
    Vec2 readMoveButtons();
    void buttonsThread();

private:
    void readGPIOs();

private:
    GameState *game;
    int chip_fd = -1;
    struct gpiohandle_request req_in;
    struct gpiohandle_data data_in;
    bool buttonLeft = 1;
    bool buttonRight = 1;
};
