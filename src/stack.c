#include "bus.h"
#include "cpu.h"
#include "stack.h"

void stack_push(u8 data) {
    bus_write(cpu_registers.sp--, data);
}

u8 stack_pop() {
    return bus_read(cpu_registers.sp++);
}

void stack_push16(u16 data) {
    u16_bytes bytes = u16_to_bytes(data);
    stack_push(bytes.hi);
    stack_push(bytes.lo);
}

u16 stack_pop16() {
    u8 lo = stack_pop();
    u8 hi = stack_pop();
    return u16_from_bytes((u16_bytes){ hi, lo });
}
