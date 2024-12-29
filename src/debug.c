#include <stdio.h>
#include "debug.h"
#include "cpu.h"
#include "mem.h"

void debug_flags() {
    printf("  ZNHC: %i%i%i%i\n", flag_z(), flag_n(), flag_h(), flag_h());
}

void debug_registers() {
    for (uint8_t i = 0; i < 8; i++) {
        printf("%02x ", mem_read(cpu_reg.pc + i));
    }
    printf("  $SP: %04x  $PC: %04x", cpu_reg.sp, cpu_reg.pc);
    printf("  $AF: %04x  $BC: %04x", cpu_reg.af, cpu_reg.bc);
    printf("  $DE: %04x  $HL: %04x", cpu_reg.de, cpu_reg.hl);
    printf("\n");
}
