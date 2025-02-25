#include "timer.h"
#include "interrupt.h"
#include "io.h"
#include "util.h"

u16 ticks = 0;

bool should_increment(u16 changed_bits, u8 clock_select) {
    switch (clock_select) {
        case 0b01: return (changed_bits & (1 << 4)) != 0;
        case 0b10: return (changed_bits & (1 << 6)) != 0;
        case 0b11: return (changed_bits & (1 << 8)) != 0;
        case 0b00: return (changed_bits & (1 << 10)) != 0;
    }

    // Unreachable
    return false;
}

void timer_tick() {
    u16 new_ticks = ticks + 1;

    // The divider register (DIV) is the upper byte of the system counter.
    // DIV is updated regardless of the `timer_enable` bit.
    io.div = u16_to_bytes(new_ticks).hi;

    // Timer Control (TAC) register
    // Bit 2 (0b0100): Timer Enable (1 = Enabled, 0 = Disabled)
    // Bits 0-1 (0b0011): Clock Select (controls the frequency at which TIMA is incremented)
    u8 timer_control = io.tac;
    bool timer_enabled = (timer_control & 0b0100) != 0;

    if (timer_enabled) {
        // Check if the counter register (TIMA) should be incremented based on the
        // selected clock frequency. The counter increments when the selected clock
        // bit transitions from 1 to 0.
        u16 changed_bits = ticks & ~new_ticks;
        u8 clock_select = timer_control & 0b0011;
        bool increment_counter = should_increment(changed_bits, clock_select);

        if (increment_counter) {
            u8 counter = io.tima + 1;
            if (counter == 0x00) {
                // If the counter overflows (0xFF -> 0x00), reset TIMA to the value of
                // the time modulo register (TMA) and request a Timer interrupt.
                io.tima = io.tma;
                cpu_request_interrupt(INTERRUPT_TIMER);
            } else {
                // Otherwise, update TIMA with the value of the incremented counter.
                io.tima = counter;
            }
        }
    }

    ticks = new_ticks;
}
