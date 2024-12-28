#include "cpu.h"
#include "debug.h"

int main(int argc, char **argv) {
    cpu_init();
    cpu_registers.a = 0x12;
    cpu_registers.f = 0x34;
    debug_registers();
    return 0;
}
