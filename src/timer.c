#include "timer.h"
#include "interrupt.h"
#include "mem.h"
#include "util.h"

u16 ticks = 0;

void timer_init() {
    ticks = 0xAC00;
    u16_bytes bytes = u16_to_bytes(ticks);
    bus.io.timer_div = bytes.hi;
}

void timer_tick() {
    ticks++;

    // tima is incremented when:
    // 1) 3rd bit of tac is set AND
    // 2) Either the 4th, 6th, 8th, or 10th bit of the timer changes
    bool update_tima = bus.io.timer_tac & 0b100;
    u8 tac_mode = bus.io.timer_tac & 0xb11;
    update_tima &=
        ((tac_mode == 0b00) & !(ticks & 0b111111111))
        | ((tac_mode == 0b01) & !(ticks & 0b111))
        | ((tac_mode == 0b10) & !(ticks & 0b11111))
        | ((tac_mode == 0b11) & !(ticks & 0b1111111));

    u8 tima = bus.io.timer_tima + update_tima;

    if (tima == 0xFF) {
        bus.io.timer_tima = bus.io.timer_tma;
        cpu_request_interrupt(INTERRUPT_TIMER);
    } else {
        bus.io.timer_tima = tima;
    }
}
