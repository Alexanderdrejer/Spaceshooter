#include "bullet.hpp"
#include <mutex>
#include <condition_variable>
#include <chrono>

Bullet::Bullet(GameState *game, Buttons *buttons): game(game), buttons(buttons) {};

void Bullet::bulletInputThread() {
    auto shootBullet = [&]
    { game->bullets.push_back({game->shipPos.x + 1, game->shipPos.y + (shipHeight / 2)}); };

    while (game->running)
    { 
        std::unique_lock<std::mutex> lock(game->gameMutex);
        game->shootButton_cv.wait(lock);
        shootBullet();
            // Note to self: wait gør to ting: Frigiver låsen og lader tråden vente på at få besked.
        
    }
}

void Bullet::bulletUpdateThread() {
    while (game->running) {
        // Running at fixed speed (bullet movement)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        {
            std::unique_lock<std::mutex> lock(game->gameMutex);
            for (auto &b : game->bullets) {
                b.x += 1;
            }

            /* TODO: Update bullets (move right)
             * 1) Is locking required? 
                    Yes since game->bullets is used by multiply threads at the same time.
                        bulletUpdateThread moves bullets. 
                        billetInputThread adds bullet.
                        Gamecontrollerthread removes bullet at collisions.
                        displayThread reads bullet.
             * 2) For each game->bulles, move them 1 px horisontally
             * 3) Is sync required?
             */       
            
            // Delete bullet when out of screen
            for (auto b = game->bullets.begin(); b != game->bullets.end(); ) {
                if (b->x > displayWidth) {
                    b = game->bullets.erase(b);  // get next valid iterator (original b invalidated by erase)
                } else {
                    ++b;  // normal increment if no erase
                }
            }
        }
    }
}
