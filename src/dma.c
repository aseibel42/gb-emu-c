#include <stdio.h>
#include "dma.h"
#include "mem.h"

bool dma_active = false;

static u8 dma_delay = 0;
static u8 dma_offset = 0;
static u8 dma_source_addr = 0;

void dma_start(u8 source_addr) {
    printf("DMA START\n");
    dma_active = true;
    dma_delay = 1;
    dma_offset = 0;
    dma_source_addr = source_addr;
}

void dma_tick() {
    if (!dma_active) {
        return;
    }

    // There is a delay of 1 clock cycle?
    if (dma_delay > 0) {
        dma_delay--;
        return;
    }

    u16 read_addr = ((u16)dma_source_addr << 8) + dma_offset;
    u16 write_addr = 0xFE00 + dma_offset;
    mem[write_addr] = mem[read_addr];

    dma_offset++;

    if (dma_offset >= 0xA0) {
        dma_active = false;
        return;
    }
}
