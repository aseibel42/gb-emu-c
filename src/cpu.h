#pragma once

#include "util.h"

#define REG(a, f) union { struct { u8 f; u8 a; }; u16 a##f; };

typedef struct {
    u8: 4; // padding
    bool c : 1;
    bool h : 1;
    bool n : 1;
    bool z : 1;
}  cpu_flags;

typedef struct cpu_registers {
    REG(a, f);
    REG(b, c);
    REG(d, e);
    REG(h, l);
    u16 pc;
    u16 sp;
} cpu_registers;

union cpu {
    cpu_flags flag;
    cpu_registers reg;
};

extern union cpu cpu;
extern bool interrupt_master_enabled;
extern bool halted;
extern bool stopped;

void cpu_init();
bool cpu_step();
void cpu_cycle();

u8 cpu_fetch();
u8 cpu_read(u16 addr);
void cpu_write(u16 addr, u8 value);

u16 cpu_fetch16();
u16 cpu_read16(u16 addr);
void cpu_write16(u16 addr, u16 value);

void cpu_jump(u16 addr);
void cpu_jmpr(i8 x);

void set_flags(i8 z, i8 n, i8 h, i8 c);

void stack_push16(u16 data);
u16 stack_pop16();
