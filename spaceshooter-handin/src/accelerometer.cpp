#include "accelerometer.hpp"
#include <chrono>
#include <thread>

Accelerometer::Accelerometer(GameState *game) : game(game)
{ 
    game->gravity = {0, 0}; 
}

// Stub: Read accelerometer over SPI
Vec2 Accelerometer::readAccelerometer() {
    // return simulated tilt as gravity vector
    return {0, 0};
}
