#include "cpu.h"
#include "debug.h"
#include "dma.h"
#include "instruction.h"
#include "interrupt.h"
#include "mem.h"
#include "stack.h"
#include "timer.h"
#include "util.h"

union cpu cpu = {};
bool interrupt_master_enabled = false;
bool halted = false;
bool stopped = false;

void cpu_init() {
    cpu.reg.pc = 0x0100;
    cpu.reg.sp = 0xFFFE;

    // TODO: initial state of CPU registers depends on type of GB
    cpu.reg.a = 0x01;
    cpu.reg.f = 0xB0;
    cpu.reg.b = 0x00;
    cpu.reg.c = 0x13;
    cpu.reg.d = 0x00;
    cpu.reg.e = 0xD8;
    cpu.reg.h = 0x01;
    cpu.reg.l = 0x4D;

    // LY register
    bus.io.lcd_y = 0x90;

    halted = false;
    interrupt_master_enabled = false;
}

bool cpu_step() {
    if (!halted) {
        // fetch instruction
        u8 opcode = fetch();

        // execute instruction
        op[opcode]();

        // check for serial output from rom
        debug_update();

        // handle interrupts (if current instruction is not ei)
        if (interrupt_master_enabled && opcode != 0xFB) {
            cpu_handle_interrupts();
        }
    } else {
        cpu_cycle();

        // resume if there is an interrupt pending
        if (bus.ie_reg & bus.io.if_reg) {
            halted = false;
        }
    }

    return true;
}

void cpu_cycle() {
    for (u8 i = 0; i < 4; i++) {
        timer_tick();
    }
    dma_tick();
}

u8 fetch() {
    return mem_read(cpu.reg.pc++);
}

u16 fetch16() {
    u16 lo = fetch();
    u16 hi = fetch();
    return u16_from_bytes((u16_bytes){ hi, lo });
}

void cpu_jump(u16 addr) {
    cpu_cycle();
    cpu.reg.pc = addr;
}

void cpu_jmpr(i8 x) {
    cpu_cycle();
    cpu.reg.pc += x;
}

void set_flags(i8 z, i8 n, i8 h, i8 c) {
    if (z != -1) cpu.flag.z = z;
    if (n != -1) cpu.flag.n = n;
    if (h != -1) cpu.flag.h = h;
    if (c != -1) cpu.flag.c = c;
}

static u16 interrupt_addr[5] = {
    ADDR_VBLANK,
    ADDR_LCD_STAT,
    ADDR_TIMER,
    ADDR_SERIAL,
    ADDR_JOYPAD
};

u16 interrupt_check(u8 interrupt_type) {
    // An interrupt is executed only if enabled (IE) and requested (IF)
    bool interrupt_pending = bit_read(bus.ie_reg & bus.io.if_reg, interrupt_type);

    if (interrupt_pending) {
        // acknowledge interrupt by clearing corresponding bit in IF register
        bus.io.if_reg = bit_clear(bus.io.if_reg, interrupt_type);
        halted = false;
        interrupt_master_enabled = false;
        return interrupt_addr[interrupt_type];
    }

    return 0;
}

void cpu_handle_interrupts() {
    u16 addr = interrupt_check(INTERRUPT_VBLANK)
        || interrupt_check(INTERRUPT_LCD_STAT)
        || interrupt_check(INTERRUPT_TIMER)
        || interrupt_check(INTERRUPT_SERIAL)
        || interrupt_check(INTERRUPT_JOYPAD);

    if (addr) {
        stack_push16(cpu.reg.pc);
        cpu_jump(addr);
    }
}

void cpu_request_interrupt(u8 type) {
    bus.io.if_reg = bit_set(bus.io.if_reg, type);
}
