#include <stdio.h>
#include "debug.h"
#include "cpu.h"
#include "instruction.h"
#include "mem.h"

void debug_flags() {
    cpu_flags f = cpu.flag;
    printf("  ZNHC: %i%i%i%i\n", f.z, f.n, f.h, f.c);
}

void debug_registers() {
    for (uint8_t i = 0; i < 8; i++) {
        printf("%02x ", mem_read(cpu.reg.pc + i));
    }
    printf("  %-12s", op_name[mem[cpu.reg.pc]]);
    printf("  $SP: %04x  $PC: %04x", cpu.reg.sp, cpu.reg.pc);
    printf("  $AF: %04x  $BC: %04x", cpu.reg.af, cpu.reg.bc);
    printf("  $DE: %04x  $HL: %04x", cpu.reg.de, cpu.reg.hl);
    // printf("\n");
}

static char dbg_msg[1024] = {};
static int msg_size = 0;

void debug_update() {
    if (bus.io.serial[1] == 0x81) {
        dbg_msg[msg_size++] = bus.io.serial[0];
        bus.io.serial[1] = 0;
        printf("DBG: %s\n", dbg_msg);
    }
}
