#include "buttons.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <thread>
#include <iostream>

/* Buttons Layout
 *                     0
 * 5  4              3   2
 *                     1
 */

/* GPIOs
 *                     12
 * 27  22            5    6
 *                     16
 */

 /* Buttons are ACTIVE LOW */

Buttons::Buttons(GameState *game): game(game)
{
    /* TODO: Request GPIO inputs
     * 1) Open gpio device
     * 2) Request inputs (IOCTL)
     */

    chip_fd = open("/dev/gpiochip0", O_RDONLY);
    if (chip_fd == -1) perror("open()"); 

    req_in.lineoffsets[0] = BTN_A;
    req_in.lineoffsets[1] = BTN_B;
    req_in.lineoffsets[2] = BTN_UP;
    req_in.lineoffsets[3] = BTN_DOWN;
    req_in.lineoffsets[4] = BTN_LEFT;
    req_in.lineoffsets[5] = BTN_RIGHT;

    req_in.flags = GPIOHANDLE_REQUEST_INPUT;
    req_in.lines = 6;
    int ret = ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req_in);
    if (ret < 0 ) perror("ioctl()");
}

Buttons::~Buttons()
{
    close(req_in.fd);
        //Lukker handleren til mine 6 pins.
    close(chip_fd);
        // lukker forbindelsen til min GPIO.
}

Vec2 Buttons::readMoveButtons()
{
    /* TODO: ReadMoveButtons
     * returns latest value of move buttons:
     * - up: {0, -1}
     * - down {0, 1}
     * - left: {-1 ,0}
     * - right: {1, 0}
     * - None pressed: {0, 0}   
     */

     if (data_in.values[2] == 0) {
     return {0,-1};

    } else if (data_in.values[3] == 0) {
        return {0, 1};

     } else if (data_in.values[4] == 0) {
        return {-1,0};

     } else if (data_in.values[5] == 0) {
        return {1,0};
     }
     else return {0,0};

}

class EdgeDetector{
private:
    int lastValue = 1;
public:
    EdgeDetector(int initialValue) : lastValue(initialValue) {};
    bool valueChanged(int value)
    {
        bool falling = (value != lastValue && value == 0);
        lastValue = value;
        return falling;
    }
};

void Buttons::buttonsThread()
{
    EdgeDetector ED_A(1);
    EdgeDetector ED_B(1);


    while (game->running)
    {

        /* TODO: Buttons Thread
         * 1) Run loop at fixed update speed @ 25 ms
         * 2) Update GPIO input data buffer
         * 3) Notify if shoot button has changed
         */

         std::this_thread::sleep_for(std::chrono::milliseconds(25));
         readGPIOs();

         if (ED_A.valueChanged(data_in.values[0]) || ED_B.valueChanged(data_in.values[1])) {
            std::unique_lock<std::mutex> lock(game->gameMutex);
                game->shootButton_cv.notify_one();
         }


    }
}

void Buttons::readGPIOs()
{
    int ret_val = ioctl(req_in.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data_in);
}