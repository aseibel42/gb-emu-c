#pragma once

#include "util.h"

u8 bus_read(u16 addr);
void bus_write(u16 addr, u8 value);

u16 bus_read16(u16 addr);
void bus_write16(u16 addr, u16 value);
