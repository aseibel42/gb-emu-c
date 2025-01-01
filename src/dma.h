#pragma once

#include "util.h"

extern bool dma_active;

void dma_start(u8 source_addr);
void dma_tick();
