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

    const int pins[NUM_BUTTONS] = { BTN_A, BTN_B, BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT };

    for (int i = 0; i < NUM_BUTTONS; ++i) {
        memset(&req_in[i], 0, sizeof(state.req[i]));
        state.req[i].lineoffset  = pins[i];
        state.req[i].eventflags  = GPIOEVENT_REQUEST_BOTH_EDGES;
        state.req[i].handleflags = GPIOHANDLE_REQUEST_INPUT;
        strcpy(state.req[i].consumer_label, "GPIO event monitor");
}

Buttons::~Buttons()
{
    /* TODO: Release GPIO inputs
     * 1) Close File descriptors (x2)
     */
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
    return {0, 0};
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
    while (game->running)
    {

        /* TODO: Buttons Thread
         * 1) Run loop at fixed update speed @ 25 ms
         * 2) Update GPIO input data buffer
         * 3) Notify if shoot button has changed
         */
    }
}

void Buttons::readGPIOs()
{
    /* TODO: Read GPIOs
     * 1) Update input data buffer with current GPIO state  
     */
}