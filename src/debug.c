#include <stdio.h>
#include "debug.h"
#include "cpu.h"
#include "instruction.h"
#include "mem.h"

void debug_flags() {
    cpu_flags flags = cpu_reg.flags;
    printf("  ZNHC: %i%i%i%i\n", flags.z, flags.n, flags.h, flags.c);
}

void debug_registers() {
    for (uint8_t i = 0; i < 8; i++) {
        printf("%02x ", mem_read(cpu_reg.pc + i));
    }
    printf("  %-12s", op_name[mem[cpu_reg.pc]]);
    printf("  $SP: %04x  $PC: %04x", cpu_reg.sp, cpu_reg.pc);
    printf("  $AF: %04x  $BC: %04x", cpu_reg.af, cpu_reg.bc);
    printf("  $DE: %04x  $HL: %04x", cpu_reg.de, cpu_reg.hl);
    // printf("\n");
}

static char dbg_msg[1024] = {};
static int msg_size = 0;

void debug_update() {
    if (mem[0xFF02] == 0x81) {
        dbg_msg[msg_size++] = mem[0xFF01];
        mem[0xFF02] = 0;
        printf("DBG: %s\n", dbg_msg);
    }
}
