#include "cpu.h"
#include "debug.h"
#include "instruction.h"
#include "interrupt.h"
#include "io.h"
#include "mem.h"
#include "ppu.h"
#include "timer.h"

union cpu cpu = {0};
bool interrupt_master_enabled = false;
bool halted = false;
bool stopped = false;

void cpu_init() {
    cpu.reg.pc = 0x0100;
    cpu.reg.sp = 0xFFFE;

    // TODO: initial state of CPU registers depends on type of GB
    cpu.reg.a = 0x01;
    cpu.reg.f = 0xB0;
    cpu.reg.b = 0x00;
    cpu.reg.c = 0x13;
    cpu.reg.d = 0x00;
    cpu.reg.e = 0xD8;
    cpu.reg.h = 0x01;
    cpu.reg.l = 0x4D;

    halted = false;
    interrupt_master_enabled = false;

    // Setup log file (gameboy doctor)
    // FILE *fp = fopen("log/cpu_instrs_02.txt", "w");
    // if (!fp) {
    //     printf("Failed to open file\n");
    //     return ptr;
    // }
}

bool cpu_step() {
    // (gameboy doctor):
    // fprintf(
    //     fp,
    //     "A:%02x F:%02x B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x SP:%04x PC:%04x PCMEM:%02x,%02x,%02x,%02x\n",
    //     cpu.reg.a, cpu.reg.f, cpu.reg.b, cpu.reg.c, cpu.reg.d, cpu.reg.e, cpu.reg.h, cpu.reg.l, cpu.reg.sp, cpu.reg.pc,
    //     mem[cpu.reg.pc], mem[cpu.reg.pc + 1], mem[cpu.reg.pc + 2], mem[cpu.reg.pc + 3]
    // );

    if (!halted) {
        // fetch instruction
        u8 opcode = cpu_fetch();

        // execute instruction
        op[opcode]();

        // check for serial output from rom
        debug_update();

        // handle interrupts (if current instruction is not ei)
        if (interrupt_master_enabled && opcode != 0xFB) {
            u16 addr = cpu_handle_interrupts();
            if (addr) {
                stack_push16(cpu.reg.pc);
                cpu_jump(addr);
            }
        }
    } else {
        cpu_cycle();

        // resume if there is an interrupt pending
        if (io.ie_reg & io.if_reg) {
            halted = false;
        }
    }

    return true;
}

void cpu_cycle() {
    for (u8 i = 0; i < 4; i++) {
        timer_tick();
        ppu_tick();
    }
    dma_tick();
}

void cpu_jump(u16 addr) {
    cpu_cycle();
    cpu.reg.pc = addr;
}

void cpu_jmpr(i8 x) {
    cpu_cycle();
    cpu.reg.pc += x;
}

u8 cpu_fetch() {
    return cpu_read(cpu.reg.pc++);
}

u8 cpu_read(u16 addr) {
    cpu_cycle();
    return mem_read(addr);
}

void cpu_write(u16 addr, u8 value) {
    cpu_cycle();
    return mem_write(addr, value);
}

u8 stack_pop() {
    return cpu_read(cpu.reg.sp++);
}

void stack_push(u8 data) {
    cpu_write(--cpu.reg.sp, data);
}

u16 cpu_fetch16() {
    u16 lo = cpu_fetch();
    u16 hi = cpu_fetch();
    return u16_from_bytes((u16_bytes){ hi, lo });
}

u16 cpu_read16(u16 addr) {
    u8 lo = cpu_read(addr);
    u8 hi = cpu_read(addr + 1);
    return u16_from_bytes((u16_bytes){ hi, lo });
}

void cpu_write16(u16 addr, u16 value) {
    u16_bytes bytes = u16_to_bytes(value);
    cpu_write(addr + 1, bytes.hi);
    cpu_write(addr, bytes.lo);
}

u16 stack_pop16() {
    u8 lo = stack_pop();
    u8 hi = stack_pop();
    return u16_from_bytes((u16_bytes){ hi, lo });
}

void stack_push16(u16 data) {
    u16_bytes bytes = u16_to_bytes(data);
    stack_push(bytes.hi);
    stack_push(bytes.lo);
}

void set_flags(i8 z, i8 n, i8 h, i8 c) {
    if (z != -1) cpu.flag.z = z;
    if (n != -1) cpu.flag.n = n;
    if (h != -1) cpu.flag.h = h;
    if (c != -1) cpu.flag.c = c;
}

bool interrupt_check(u8 interrupt_type) {
    // An interrupt is executed only if enabled (IE) and requested (IF)
    bool interrupt_pending = bit_read(io.ie_reg & io.if_reg, interrupt_type);

    if (interrupt_pending) {
        // acknowledge interrupt by clearing corresponding bit in IF register
        io.if_reg = bit_clear(io.if_reg, interrupt_type);
        halted = false;
        interrupt_master_enabled = false;
    }

    return interrupt_pending;
}

u16 cpu_handle_interrupts() {
    if (interrupt_check(INTERRUPT_VBLANK)) return ADDR_VBLANK;
    if (interrupt_check(INTERRUPT_LCD_STAT)) return ADDR_LCD_STAT;
    if (interrupt_check(INTERRUPT_TIMER)) return ADDR_TIMER;
    if (interrupt_check(INTERRUPT_SERIAL)) return ADDR_SERIAL;
    if (interrupt_check(INTERRUPT_JOYPAD)) return ADDR_JOYPAD;
    return 0;
}

void cpu_request_interrupt(u8 type) {
    io.if_reg = bit_set(io.if_reg, type);
}
