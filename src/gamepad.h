#pragma once

#include "util.h"

struct gamepad {
    union {
        struct {
            u8 a: 1;
            u8 b: 1;
            u8 select: 1;
            u8 start: 1;
            u8 : 1;
            u8 select_btns: 1;
        };
        struct {
            u8 right: 1;
            u8 left: 1;
            u8 up: 1;
            u8 down: 1;
            u8 select_dpad: 1;
        };
    };
};

void gamepad_init();
