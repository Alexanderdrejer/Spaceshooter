#include "I2Cdriver.hpp"
#include "SSD1306_OLED.hpp"
#include "display.hpp"
#include "buttons.hpp"
#include "game_controller.hpp"
#include "accelerometer.hpp"
#include "enemy.hpp"
#include "bullet.hpp"
#include "ship.hpp"

#define myOLEDwidth 128
#define myOLEDheight 32
#define FULLSCREEN (myOLEDwidth * (myOLEDheight / 8))
int main(int argc, char *argv[]) {
  I2CDriver i2cDriver;
  SSD1306 oled(myOLEDwidth, myOLEDheight, i2cDriver);
  uint8_t screenBuffer[FULLSCREEN];
  if (!oled.OLEDSetBufferPtr(myOLEDwidth, myOLEDheight, screenBuffer, sizeof(screenBuffer)))
  return -1;
  oled.OLEDbegin(0x3c); // initialize the OLED

  GameState game;
  Buttons buttons(&game);
  Display display(oled, &game);
  Ship ship(&game, &buttons);
  Accelerometer accl(&game);
  GameController game_ctrl(&game);
  Enemy enemy(&game);
  Bullet bullet(&game, &buttons);

  // Threads need function object from non-static member function
  std::thread t1([&buttons]() { buttons.buttonsThread(); });
  std::thread t2([&game_ctrl]() { game_ctrl.gameControllerThread(); });
  std::thread t3([&display]() { display.displayThread(); });
  std::thread t4([&enemy]() { enemy.enemyThread(); });
  std::thread t5([&ship]() { ship.shipThread(); });
  std::thread t6([&bullet]() { bullet.bulletInputThread(); });
  std::thread t7([&bullet]() { bullet.bulletUpdateThread(); });
  
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  t6.join();
  t7.join();
 
  //oled.OLEDPowerDown();

  return 0;
}
