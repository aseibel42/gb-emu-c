#pragma once

#include "util.h"

#define REG(a, f) union { struct { u8 a; u8 f; }; u16 a##f; };

struct cpu_registers {
    u16 pc;
    u16 sp;
    REG(a, f);
    REG(b, c);
    REG(d, e);
    REG(h, l);
};

struct cpu_state {
    u8 interrupt_register;
    u8 interrupt_flags;
    bool enabling_interrupt_master;
    bool interrupt_master_enabled;
    bool halted;
    bool stepping;
};

extern struct cpu_registers cpu_reg;
extern struct cpu_state cpu_state;

void cpu_init();
bool cpu_step();

u8 fetch();
u16 fetch16();

bool flag_z();
bool flag_n();
bool flag_h();
bool flag_c();

void set_flags(i8 z, i8 n, i8 h, i8 c);
