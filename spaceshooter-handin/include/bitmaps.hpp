#pragma once
#include <cstdint>

/* Display is addressed as follows:
 *  (0,0)..........................(0,127)
 *       ..........................
 *       ..........................
 *       ..........................
 * (31,0)..........................(31,127)
 */

// 8x7 pixel ship bitmap (monochrome, horizontally addressed)
const uint8_t shipBitmap[] = {
    0xE0, // ###.....
    0xF8, // #####...
    0xFB, // ######..
    0xFF, // ########
    0xFB, // ######..
    0xF8, // #####...
    0xE0  // ###.....
};

// 8x1 bullet bitmap
const uint8_t bulletBitmap8x1[] = {
    0x55 // .#.#.#.#
};

// 8x7 enemy bitmap
const uint8_t enemyBitmap8x8[] = {
    0x38, // ..###...
    0xFF, // ########
    0x33, // ..##..##
    0xFF, // ########
    0xFF, // ########
    0x66, // .##..##.
    0x66  // .##..##.
};