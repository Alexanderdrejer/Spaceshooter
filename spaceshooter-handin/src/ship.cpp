#include "ship.hpp"
#include <mutex>
#include <chrono>
#include <thread>

Ship::Ship(GameState *game, Buttons *buttons): game(game), buttons(buttons) {};

void Ship::moveShip(Vec2 move)
{
    {
        game->shipPos.x += move.x;
        if (game->shipPos.x < 0)
            game->shipPos.x = 0;
        if (game->shipPos.x > displayWidth-1-shipWidth)
            game->shipPos.x = displayWidth-1-shipWidth;
        game->shipPos.y += move.y;
        if (game->shipPos.y < 0)
            game->shipPos.y = 0;
        if (game->shipPos.y > displayHeight-1-(shipHeight/2))
            game->shipPos.y = displayHeight-1-(shipHeight/2);
    }
}

void Ship::shipThread() {
    while (game->running) 
    {
        // Fixed speed
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // Ensure display is not updated in the middle
        // of updating position
        std::lock_guard<std::mutex> lock(game->gameMutex);
        moveShip(buttons->readMoveButtons());
    }
}