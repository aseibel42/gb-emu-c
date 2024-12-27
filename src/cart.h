#pragma once

#include "util.h"

typedef struct {
    u8 entry[4];
    u8 logo[48];
    u8 title[16];
    u16 licensee_code_new;
    u8 sgb_flag;
    u8 cartridge_type;
    u8 rom_size;
    u8 ram_size;
    u8 dest_code;
    u8 licensee_code_old;
    u8 version;
    u8 header_checksum;
    u16 global_checksum;
} rom_header;

bool cart_load(u8 *cart);

u8 cart_read(u16 addr);
void cart_write(u16 addr, u8 value);
