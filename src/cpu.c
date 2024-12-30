#include "cpu.h"
#include "debug.h"
#include "emu.h"
#include "instruction.h"
#include "interrupt.h"
#include "mem.h"
#include "stack.h"
#include "timer.h"
#include "util.h"

struct cpu_registers cpu_reg = {};
bool interrupt_master_enabled = false;
bool halted = false;
bool stopped = false;

void cpu_init() {
    cpu_reg.pc = 0x0100;
    cpu_reg.sp = 0xFFFE;

    // TODO: initial state of CPU registers depends on type of GB
    cpu_reg.a = 0x01;
    cpu_reg.f = 0xB0;
    cpu_reg.b = 0x00;
    cpu_reg.c = 0x13;
    cpu_reg.d = 0x00;
    cpu_reg.e = 0xD8;
    cpu_reg.h = 0x01;
    cpu_reg.l = 0x4D;

    // LY register
    mem[0xFF44] = 0x90;

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
        if (ie_register() & if_register()) {
            halted = false;
        }
    }

    return true;
}

void cpu_cycle() {
    for (u8 i = 0; i < 4; i++) {
        ticks++;
        timer_tick();
    }
}

u8 fetch() {
    return mem_read(cpu_reg.pc++);
}

u16 fetch16() {
    u16 lo = fetch();
    u16 hi = fetch();
    return u16_from_bytes((u16_bytes){ hi, lo });
}

void cpu_jump(u16 addr) {
    cpu_cycle();
    cpu_reg.pc = addr;
}

void cpu_jmpr(i8 x) {
    cpu_cycle();
    cpu_reg.pc += x;
}

bool flag_z() {
    return bit_read(cpu_reg.f, 7);
}

bool flag_n() {
    return bit_read(cpu_reg.f, 6);
}

bool flag_h() {
    return bit_read(cpu_reg.f, 5);
}

bool flag_c() {
    return bit_read(cpu_reg.f, 4);
}

void set_flags(i8 z, i8 n, i8 h, i8 c) {
    if (z != -1) cpu_reg.f = bit_assign(cpu_reg.f, 7, z);
    if (n != -1) cpu_reg.f = bit_assign(cpu_reg.f, 6, n);
    if (h != -1) cpu_reg.f = bit_assign(cpu_reg.f, 5, h);
    if (c != -1) cpu_reg.f = bit_assign(cpu_reg.f, 4, c);
}

bool interrupt_check(u8 interrupt_type) {
    // An interrupt is executed only if enabled (IE) and requested (IF)
    bool interrupt_pending = true;
    interrupt_pending &= bit_read(ie_register(), interrupt_type); // IE register
    interrupt_pending &= bit_read(if_register(), interrupt_type); // IF register

    if (interrupt_pending) {
        // acknowledge interrupt by clearing corresponding bit in IF register
        mem[0xFF0F] = bit_clear(mem[0xFF0F], interrupt_type);
        halted = false;
        interrupt_master_enabled = false;
    }

    return interrupt_pending;
}

void cpu_handle_interrupts() {
    if (interrupt_check(INTERRUPT_VBLANK)) {
        stack_push16(cpu_reg.pc);
        cpu_jump(ADDR_VBLANK);
    } else if (interrupt_check(INTERRUPT_LCD_STAT)) {
        stack_push16(cpu_reg.pc);
        cpu_jump(ADDR_LCD_STAT);
    } else if (interrupt_check(INTERRUPT_TIMER)) {
        stack_push16(cpu_reg.pc);
        cpu_jump(ADDR_TIMER);
    } else if (interrupt_check(INTERRUPT_SERIAL)) {
        stack_push16(cpu_reg.pc);
        cpu_jump(ADDR_SERIAL);
    } else if (interrupt_check(INTERRUPT_JOYPAD)) {
        stack_push16(cpu_reg.pc);
        cpu_jump(ADDR_JOYPAD);
    }
}

void cpu_request_interrupt(u8 type) {
    mem[0xFF0F] = bit_set(mem[0xFF0F], type);
}

u8 ie_register() {
    return mem[0xFFFF];
}

u8 if_register() {
    return mem[0xFF0F];
}
