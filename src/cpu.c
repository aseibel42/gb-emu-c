#include "cpu.h"
#include "instruction.h"
#include "mem.h"

struct cpu_registers cpu_reg = {};
struct cpu_state cpu_state = {};

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

    cpu_state.interrupt_register = 0;
    cpu_state.interrupt_flags = 0;
}

bool cpu_step() {
    // fetch instruction
    u8 opcode = fetch();

    // execute instruction
    op[opcode]();

    return false;
}

u8 fetch() {
    return mem_read(cpu_reg.pc++);
}

u16 fetch16() {
    u16 lo = fetch();
    u16 hi = fetch();
    return u16_from_bytes((u16_bytes){ hi, lo });
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
    if (z != -1) bit_assign(cpu_reg.f, 7, z);
    if (n != -1) bit_assign(cpu_reg.f, 6, n);
    if (h != -1) bit_assign(cpu_reg.f, 5, h);
    if (c != -1) bit_assign(cpu_reg.f, 4, c);
}
