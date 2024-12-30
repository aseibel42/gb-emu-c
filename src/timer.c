#include "timer.h"
#include "interrupt.h"
#include "mem.h"
#include "util.h"

u16 counter = 0;

void timer_init() {
    counter = 0xAC00;
    u16_bytes bytes = u16_to_bytes(counter);
    timer_set_div(bytes.hi);
}

void timer_tick() {
    counter++;

    u8 tac = timer_tac();

    // tima is incremented when:
    // 1) 3rd bit of tac is set AND
    // 2) Either the 4th, 6th, 8th, or 10th bit of the timer changes
    bool update_tima = tac & 0b100;
    u8 tac_mode = tac & 0xb11;
    update_tima &=
        (tac_mode == 0b00) & !(counter & 0b111111111)
        | (tac_mode == 0b01) & !(counter & 0b111)
        | (tac_mode == 0b10) & !(counter & 0b11111)
        | (tac_mode == 0b11) & !(counter & 0b1111111);

    u8 tima = timer_tima() + update_tima;

    if (tima == 0xFF) {
        timer_set_tima(timer_tma());
        cpu_request_interrupt(INTERRUPT_TIMER);
    } else {
        timer_set_tima(tima);
    }
}

u8 timer_div() { return mem[0xFF04]; }
u8 timer_tima() { return mem[0xFF05]; }
u8 timer_tma() { return mem[0xFF06]; }
u8 timer_tac() { return mem[0xFF07]; }

void timer_set_div(u8 value) { mem[0xFF04] = value; }
void timer_set_tima(u8 value) { mem[0xFF05] = value; }
void timer_set_tma(u8 value) { mem[0xFF06] = value; }
void timer_set_tac(u8 value) { mem[0xFF07] = value; }
