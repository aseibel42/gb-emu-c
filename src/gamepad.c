#include "gamepad.h"
#include "mem.h"

union button_state btns = {0};

void gamepad_init() {
    // Unconventionally, a set bit means "off" or "not pressed" for the gamepad
    // Initially, each flag in the register is set to "off"
    mem[0xFF00] = 0x3f;
    btns.a = 1;
    btns.b = 1;
    btns.select = 1;
    btns.start = 1;
    btns.right = 1;
    btns.left = 1;
    btns.up = 1;
    btns.down = 1;
}
