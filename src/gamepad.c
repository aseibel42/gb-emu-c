#include "gamepad.h"
#include "mem.h"

void gamepad_init() {
    // Unconventionally, a set bit means "off" or "not pressed" for the gamepad
    // Initially, each flag in the register is set to "off"
    mem[0xFF00] = 0x3f;
}
