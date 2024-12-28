#include <stdio.h>
#include "debug.h"
#include "cpu.h"
#include "mem.h"

void debug_registers() {
    for (uint8_t i = 0; i < 8; i++) {
        printf("%02x ", mem_read(cpu_registers.pc + i));
    }
    printf("  $SP: %04x  $PC: %04x", cpu_registers.sp, cpu_registers.pc);
    printf("  $AF: %04x  $BC: %04x", cpu_registers.af, cpu_registers.bc);
    printf("  $DE: %04x  $HL: %04x", cpu_registers.de, cpu_registers.hl);
    printf("  ZNHC: %i%i%i%i\n", flag_z(), flag_n(), flag_h(), flag_h());
}
