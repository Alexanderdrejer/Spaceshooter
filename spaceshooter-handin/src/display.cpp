#include "display.hpp"
#include <thread>
#include <iostream>

Display::Display(SSD1306 &oledRef, GameState *game) : oled(oledRef), game(game) {}


void Display::drawDisplay(const Vec2 &shipPos,
                         const std::vector<Vec2> &bullets,
                         const std::vector<Vec2> &enemies,
                         int score) {

    // Clear display buffer first
    oled.OLEDclearBuffer();

    // Draw ship (anchor at top-left corner of sprite)
    oled.OLEDBitmap(shipPos.x, shipPos.y, shipWidth, shipHeight, shipBitmap, false);

    // Draw bullets
    for (auto &b : bullets) {
        oled.OLEDBitmap(b.x, b.y, bulletWidth, bulletHeight, bulletBitmap8x1, false);
    }

    // Draw enemies
    for (auto &e : enemies) {
        oled.OLEDBitmap(e.x, e.y, enemyWidth, enemyHeight, enemyBitmap8x8, false);
    }

    //  Render score as text
    oled.setTextColor(WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.print("S ");
    oled.print(score);
    oled.print(" B ");
    oled.print(bullets.size());
    oled.print(" E ");
    oled.print(enemies.size());

    // Send buffer to OLED
    oled.OLEDupdate();
}

void Display::displayThread() {
    while (game->running) {
        // Fixed update speed
        std::this_thread::sleep_for(std::chrono::milliseconds(25));

        GameState snapshot;
        {
            std::lock_guard<std::mutex> lock(game->gameMutex);
            snapshot.shipPos = game->shipPos;
            snapshot.bullets = game->bullets;
            snapshot.enemies = game->enemies;
            snapshot.score = game->score;
        }
        drawDisplay(snapshot.shipPos, snapshot.bullets, snapshot.enemies, snapshot.score);
    }

    // When game is over...
    oled.OLEDclearBuffer();
    oled.setTextColor(WHITE);
    oled.setTextSize(2);
    oled.setCursor(10, 10);
    oled.print("GAME OVER");
    oled.OLEDupdate();
 
}
