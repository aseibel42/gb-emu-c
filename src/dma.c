#include "dma.h"
#include "mem.h"

bool dma_active = false;

static u8 dma_delay = 0;
static u8 dma_offset = 0;
static u16 dma_source_addr = 0;

void dma_start(u8 source_addr) {
    dma_active = true;
    dma_delay = 1;
    dma_offset = 0;
    dma_source_addr = source_addr << 8;
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

    // Copy a single byte to Object Attribute Memory
    // TODO: if timings don't matter, a memcpy would be more efficient than copying one value at a time
    bus.oam[dma_offset] = mem_read(dma_source_addr + dma_offset);

    dma_offset++;

    if (dma_offset >= 0xA0) {
        dma_active = false;
        return;
    }
}
