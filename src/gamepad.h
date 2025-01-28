#pragma once

#include "util.h"

union button_state {
    struct {
        u8 a: 1;
        u8 b: 1;
        u8 select: 1;
        u8 start: 1;
        u8 right: 1;
        u8 left: 1;
        u8 up: 1;
        u8 down: 1;
    };
    struct {
        u8 ctrl: 4;
        u8 dpad: 4;
    };
};

struct gamepad {
    u8 : 4;
    u8 select_dpad: 1;
    u8 select_btns: 1;
};

void gamepad_init();

extern union button_state btns;
