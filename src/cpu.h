#pragma once

#include "util.h"

struct cpu_registers {
    u8 a;
    u8 f;
    u8 b;
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 pc;
    u16 sp;
};

struct cpu_state {
    u8 interrupt_register;
    u8 interrupt_flags;
    bool enabling_interrupt_master;
    bool interrupt_master_enabled;
    bool halted;
    bool stepping;
};

extern struct cpu_registers cpu_registers;
extern struct cpu_state cpu_state;

void cpu_init();
bool cpu_step();

u16 cpu_read_reg_af();
u16 cpu_read_reg_bc();
u16 cpu_read_reg_de();
u16 cpu_read_reg_hl();

void cpu_set_reg_af(u16);
void cpu_set_reg_bc(u16);
void cpu_set_reg_de(u16);
void cpu_set_reg_hl(u16);

bool cpu_read_flag_z();
bool cpu_read_flag_n();
bool cpu_read_flag_h();
bool cpu_read_flag_c();

void cpu_set_flag_z(bool);
void cpu_set_flag_n(bool);
void cpu_set_flag_h(bool);
void cpu_set_flag_c(bool);

static inline u16 read_r16(u16* ptr) {
    return *ptr;
}

static inline void set_r16(u16* ptr, u16 value) {
    *ptr = value;
}
