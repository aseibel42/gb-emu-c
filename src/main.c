#include "cpu.h"
#include "debug.h"
#include "mem.h"

int main(int argc, char **argv) {
    cpu_init();
    mem_write(cpu_reg.pc, 0x41);
    cpu_step();
    mem_write(cpu_reg.pc, 0x80);
    cpu_step();
    debug_registers();
    return 0;
}
