#include "bus.h"
#include "cpu.h"
#include "instruction.h"

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
    u8 opcode = bus_read(cpu_registers.pc++);
    instruction instr = instructions[opcode];

    // fetch data
    u16 fetched_data = 0;
    u16 memory_dest = 0;

    // execute instruction

    return false;
}

u16 cpu_read_reg_af() {
    return read_r16((u16*)&cpu_registers.a);
}

u16 cpu_read_reg_bc() {
    return read_r16((u16*)&cpu_registers.b);
}

u16 cpu_read_reg_de() {
    return read_r16((u16*)&cpu_registers.d);
}

u16 cpu_read_reg_hl() {
    return read_r16((u16*)&cpu_registers.h);
}

void cpu_set_reg_af(u16 value) {
    set_r16((u16*)&cpu_registers.a, value);
}

void cpu_set_reg_bc(u16 value) {
    set_r16((u16*)&cpu_registers.b, value);
}

void cpu_set_reg_de(u16 value) {
    set_r16((u16*)&cpu_registers.d, value);
}

void cpu_set_reg_hl(u16 value) {
    set_r16((u16*)&cpu_registers.h, value);
}

bool cpu_read_flag_z(){
    return bit_read(cpu_registers.f, 7);
}

bool cpu_read_flag_n(){
    return bit_read(cpu_registers.f, 6);
}

bool cpu_read_flag_h(){
    return bit_read(cpu_registers.f, 5);
}

bool cpu_read_flag_c(){
    return bit_read(cpu_registers.f, 4);
}

void cpu_set_flag_z(bool flag) {
    cpu_registers.f = bit_assign(cpu_registers.f, 7, flag);
}

void cpu_set_flag_n(bool flag) {
    cpu_registers.f = bit_assign(cpu_registers.f, 6, flag);
}

void cpu_set_flag_h(bool flag) {
    cpu_registers.f = bit_assign(cpu_registers.f, 5, flag);
}

void cpu_set_flag_c(bool flag) {
    cpu_registers.f = bit_assign(cpu_registers.f, 4, flag);
}
