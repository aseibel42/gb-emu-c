#pragma once

#include "util.h"

extern u16 counter;

void timer_init();
void timer_tick();

u8 timer_read(u16 addr);
void timer_write(u16 addr, u8 value);

u8 timer_div();
u8 timer_tima();
u8 timer_tma();
u8 timer_tac();

void timer_set_div(u8);
void timer_set_tima(u8);
void timer_set_tma(u8);
void timer_set_tac(u8);
