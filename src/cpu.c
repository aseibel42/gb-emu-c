#include "cpu.h"
#include "instruction.h"
#include "mem.h"

struct cpu_registers cpu_registers = {};
struct cpu_state cpu_state = {};

void cpu_init() {
    cpu_registers.pc = 0x0100;
    cpu_registers.sp = 0xFFFE;

    // TODO: initial state of CPU registers depends on type of GB
    cpu_registers.a = 0x01;
    cpu_registers.f = 0xB0;
    cpu_registers.b = 0x00;
    cpu_registers.c = 0x13;
    cpu_registers.d = 0x00;
    cpu_registers.e = 0xD8;
    cpu_registers.h = 0x01;
    cpu_registers.l = 0x4D;

    cpu_state.interrupt_register = 0;
    cpu_state.interrupt_flags = 0;
}

bool cpu_step() {
    // fetch instruction
    u8 opcode = mem_read(cpu_registers.pc++);
    instruction instr = instructions[opcode];

    // fetch data
    u16 fetched_data = 0;
    u16 memory_dest = 0;

    // execute instruction

    return false;
}

bool flag_z() {
    return bit_read(cpu_registers.f, 7);
}

bool flag_n() {
    return bit_read(cpu_registers.f, 6);
}

bool flag_h() {
    return bit_read(cpu_registers.f, 5);
}

bool flag_c() {
    return bit_read(cpu_registers.f, 4);
}

void set_flags(i8 z, i8 n, i8 h, i8 c) {
    if (z != -1) bit_assign(cpu_registers.f, 7, z);
    if (n != -1) bit_assign(cpu_registers.f, 6, n);
    if (h != -1) bit_assign(cpu_registers.f, 5, h);
    if (c != -1) bit_assign(cpu_registers.f, 4, c);
}
