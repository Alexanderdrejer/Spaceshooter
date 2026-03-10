#pragma once
#include "bitmaps.hpp"
#include "game_types.hpp"
#include "game_state.hpp"
#include "SSD1306_OLED.hpp"
#include <vector>

class Display {
public:
    Display(SSD1306 &oledRef, GameState *game);
    void drawDisplay(const Vec2 &shipPos,
                     const std::vector<Vec2> &bullets,
                     const std::vector<Vec2> &enemies,
                     int score);
    void displayThread();

private:
    SSD1306 &oled;
    GameState *game;
};
