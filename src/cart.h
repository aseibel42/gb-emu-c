#pragma once

#include "util.h"

typedef struct {
    u8 entry[4];
    u8 logo[48];
    union {
        u8 title[16];
        struct {
            u32 : 32; // padding
            u32 : 32; // padding
            u16 : 16; // padding
            u8 : 8;   // padding
            u8 manufacturer_code[4];
            u8 cgb_flag;
        };
    };
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

typedef struct {
    size_t rom_size;
    u8 *rom;
    size_t ram_size;
    u8 *ram;
} Cart;

extern Cart cart;
void load_rom(const char *filename);
