#pragma once

#include "util.h"

#define ROM_BANK_SIZE 16384
#define RAM_BANK_SIZE 8192

#define ROM_ONLY 0
#define MBC1 1
#define MBC2 2
#define MBC3 3
#define MBC5 5

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
    u8 cart_type;
    u8 rom_size;
    u8 ram_size;
    u8 dest_code;
    u8 licensee_code_old;
    u8 version;
    u8 header_checksum;
    u16 global_checksum;
} rom_header;

typedef u8 (*Read)(u16);
typedef void (*Write)(u16, u8);

typedef struct {
    u8 *rom;
    u8 *ram;
    Write set_mbc_reg;
    Read read_ram;
    Write write_ram;
    u16 num_rom_banks;
    u8 num_ram_banks;
    u8 mbc_type;
    u8 rom_bank;
    u8 ram_bank;
    bool ram_enable;
    bool mbc_mode;
} Cart;

extern Cart cart;
void load_rom(const char *filename);
