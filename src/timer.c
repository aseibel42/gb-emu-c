#include "timer.h"
#include "interrupt.h"
#include "io.h"
#include "util.h"

u16 ticks = 0;

void timer_tick() {
    ticks++;
    io.div = u16_to_bytes(ticks).hi;

    // tima is incremented when:
    // 1) 3rd bit of tac is set AND
    // 2) Either the 4th, 6th, 8th, or 10th bit of the timer changes
    bool update_tima = io.tac & 0b100;
    u8 tac_mode = io.tac & 0xb11;
    update_tima &=
        ((tac_mode == 0b00) & !(ticks & 0b111111111))
        | ((tac_mode == 0b01) & !(ticks & 0b111))
        | ((tac_mode == 0b10) & !(ticks & 0b11111))
        | ((tac_mode == 0b11) & !(ticks & 0b1111111));

    u8 tima = io.tima + update_tima;

    if (tima == 0xFF) {
        io.tima = io.tma;
        cpu_request_interrupt(INTERRUPT_TIMER);
    } else {
        io.tima = tima;
    }
}
