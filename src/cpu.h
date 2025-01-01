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

struct cpu_registers {
    union {
        struct {
            union {
                u8 f;
                cpu_flags flags;
            };
            u8 a;
        };
        u16 af;
    };

    REG(b, c);
    REG(d, e);
    REG(h, l);

    u16 pc;
    u16 sp;
};

extern struct cpu_registers cpu_reg;
extern bool interrupt_master_enabled;
extern bool halted;
extern bool stopped;

void cpu_init();
bool cpu_step();
void cpu_cycle();

u8 fetch();
u16 fetch16();

void cpu_jump(u16 addr);
void cpu_jmpr(i8 x);

bool flag_z();
bool flag_n();
bool flag_h();
bool flag_c();

void set_flags(i8 z, i8 n, i8 h, i8 c);

u8 ie_register();
u8 if_register();
