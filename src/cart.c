#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cart.h"
#include "mem.h"

Cart cart = {0};
char stem[256];
char save[256];

// RAM size lookup table
static const size_t ram_size_table[] = {
    0,  // 0x00:   0 (no RAM)
    0,  // 0x01:  -- (unused)
    1,  // 0x02:   8 KB
    4,  // 0x03:  32 KB
    16, // 0x04: 128 KB
    8,  // 0x05:  64 KB
};

bool is_cgb() {
    rom_header header = *(rom_header*)(cart.rom + 0x100);
    return (header.cgb_flag & 0x80) != 0;
}

u8 mbc_read_ram(u16 addr) {
    return bus.sram[addr];
}

void mbc_write_ram(u16 addr, u8 value) {
    bus.sram[addr] = value;
}

// MBC2 is limited to 512 bytes of half-byte RAM
// Only the lower nibble can be read
u8 mbc2_read_ram(u16 addr) {
    return bus.sram[addr & 0x1FF] & 0xF;
}

void mbc2_write_ram(u16 addr, u8 value) {
    bus.sram[addr & 0x1FF] = value;
}

// MBC1 is odd because the 2-bit ram_bank register is used for both the
// ram_bank _and_ rom_0, but only if mbc.mode is 1
void mbc1_reg(u16 addr, u8 value) {
    if (addr < 0x2000) {
        cart.ram_enable = (value & 0xF) == 0xA;
    } else {
        if (addr < 0x4000) {
            cart.rom_bank = value & 0x1F;
            cart.rom_bank += !cart.rom_bank;
        } else {
            if (addr < 0x6000) {
                cart.ram_bank = value & 0x03;
            } else if (addr < 0x8000) {
                cart.mbc_mode = value & 0x01;
            }
            bus.sram = cart.ram + cart.mbc_mode * (cart.ram_bank * SRAM_BANK_SIZE);
        }

        // update rom pointers
        u8 mask = cart.num_rom_banks - 1;
        u8 rom_bank_0 = cart.mbc_mode * (cart.ram_bank << 5);
        u8 rom_bank_1 = cart.rom_bank | (cart.ram_bank << 5);
        bus.rom_0 = cart.rom + (rom_bank_0 & mask) * ROM_BANK_SIZE;
        bus.rom_1 = cart.rom + (rom_bank_1 & mask) * ROM_BANK_SIZE;
    }
}

void mbc2_reg(u16 addr, u8 value) {
    if (addr < 0x3FFF) {
        if (bit_read(u16_to_bytes(addr).hi, 0)) {
            cart.rom_bank = value & 0xF;
            cart.rom_bank += !cart.rom_bank;
            bus.rom_1 = cart.rom + cart.rom_bank * ROM_BANK_SIZE;
        } else {
            cart.ram_enable = value == 0x0A;
        }
    }
}

void mbc3_reg(u16 addr, u8 value) {
    if (addr < 0x2000) {
        if (value == 0x0A) {
            cart.ram_enable = true;
        } else if (!value) {
            cart.ram_enable = false;
        }
    } else if (addr < 0x4000) {
        cart.rom_bank = value & 0x7F;
        cart.rom_bank += !cart.rom_bank;
        bus.rom_1 = cart.rom + cart.rom_bank * ROM_BANK_SIZE;
    } else if (addr < 0x6000) {
        cart.ram_bank = value & 0x03;
        bus.sram = cart.ram + cart.ram_bank * SRAM_BANK_SIZE;
    } else if (addr < 0x8000) {
        // Latch clock data?
    }
}

// MBC5 is similar to MBC3, with the `mode` register repurposed as the 9th bit of rom_bank
void mbc5_reg(u16 addr, u8 value) {
    if (addr < 0x2000) {
        if (value == 0x0A) {
            cart.ram_enable = true;
        } else if (!value) {
            cart.ram_enable = false;
        }
    } else if (addr < 0x3000) {
        cart.rom_bank = value;
        bus.rom_1 = cart.rom + (cart.rom_bank | (cart.mbc_mode << 8)) * ROM_BANK_SIZE;
    } else if (addr < 0x4000) {
        cart.mbc_mode = value & 0x01;
        bus.rom_1 = cart.rom + (cart.rom_bank | (cart.mbc_mode << 8)) * ROM_BANK_SIZE;
    } else if (addr < 0x6000) {
        cart.ram_bank = value & 0x0F;
        bus.sram = cart.ram + cart.ram_bank * SRAM_BANK_SIZE;
    }
}

void cart_load(char *filename) {
    get_stem(filename);
    cart.name = stem;
    sprintf(save, "save/%s.bin", cart.name);
    printf("Loading ROM: %s\n", cart.name);

    // read file
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open ROM file\n");
        return;
    }

    // seek to ROM header at 0x100
    if (fseek(file, 0x100, SEEK_SET)) {
        perror("Failed to seek to ROM header\n");
        goto close;
    }

    // read ROM header
    rom_header header;
    if (fread(&header, sizeof(rom_header), 1, file) != 1) {
        perror("Failed to read ROM header\n");
        goto close;
    }
    printf("Title: %.*s\n", 16, header.title);

    // validate ROM size
    if (header.rom_size > 8) {
        fprintf(stderr, "Unsupported ROM size code: 0x%02X\n", header.rom_size);
        goto close;
    }

    // allocate memory for ROM
    cart.num_rom_banks = 2 << header.rom_size;
    cart.rom = malloc(cart.num_rom_banks * ROM_BANK_SIZE);
    if (!cart.rom) {
        perror("Failed to allocate memory for ROM\n");
        goto close;
    }
    printf("ROM: %d bytes\n", cart.num_rom_banks * ROM_BANK_SIZE);

    // read ROM
    rewind(file);
    if (fread(cart.rom, ROM_BANK_SIZE, cart.num_rom_banks, file) != cart.num_rom_banks) {
        perror("Failed to read ROM data\n");
        goto cleanup_rom;
    }

    // validate RAM size
    if (header.ram_size == 1 || header.ram_size > 5) {
        fprintf(stderr, "Unsupported RAM size code: 0x%02X\n", header.ram_size);
        goto cleanup_rom;
    }

    // allocate memory for RAM
    cart.num_ram_banks = ram_size_table[header.ram_size];
    if (cart.num_ram_banks > 0) {
        cart.ram = malloc(cart.num_ram_banks * SRAM_BANK_SIZE);
        if (!cart.ram) {
            perror("Failed to allocate memory for RAM\n");
            goto cleanup_ram;
        }
        printf("RAM: %d bytes\n", cart.num_ram_banks * SRAM_BANK_SIZE);
    }

    // hookup memory regions to bus
    bus.rom_0 = cart.rom;
    bus.rom_1 = cart.rom + ROM_BANK_SIZE;
    bus.sram = cart.ram;

    // configure memory bank controller (MBC)
    if (header.cart_type == 0x00) {
        // No MBC (ROM only)
    } else if (header.cart_type <= 0x03) {
        // MBC1
        cart.mbc_type = MBC1;
        cart.set_mbc_reg = mbc1_reg;
        cart.read_ram = mbc_read_ram;
        cart.write_ram = mbc_write_ram;
    } else if (header.cart_type <= 0x06) {
        // MBC2 (no external RAM)
        cart.mbc_type = MBC2;
        cart.rom_bank = 1;
        cart.set_mbc_reg = mbc2_reg;
        cart.read_ram = mbc2_read_ram;
        cart.write_ram = mbc2_write_ram;
    } else if (header.cart_type <= 0x09) {
        // No MBC (ROM + RAM)
    } else if (header.cart_type <= 0x0D) {
        // MMM01 (Not implemented)
    } else if (header.cart_type <= 0x13) {
        // MBC3
        cart.mbc_type = MBC3;
        cart.set_mbc_reg = mbc3_reg;
        cart.read_ram = mbc_read_ram;
        cart.write_ram = mbc_write_ram;
    } else if (header.cart_type <= 0x1E) {
        // MBC5
        cart.mbc_type = MBC5;
        cart.set_mbc_reg = mbc5_reg;
        cart.read_ram = mbc_read_ram;
        cart.write_ram = mbc_write_ram;
    }

    // everything has succeeded - return to skip cleanup
    return;

// NOTE: use of labels and goto statements is generally discouraged, but it
// _is_ useful for resource management like closing files and freeing memory
// when the process can fail at multiple different stages.
cleanup_ram:
    free(cart.ram);
    cart.ram = NULL;
cleanup_rom:
    free(cart.rom);
    cart.rom = NULL;
close:
    fclose(file);
}

void get_stem(char* path) {
    // Find the last occurrence of "/"
    char *dir_pos = strrchr(path, '/');

    // Filename begins after the slash
    const char *filename = dir_pos ? dir_pos + 1 : path;

    // Find the last occurrence of "."
    const char *ext_pos = strrchr(path, '.');

    // Calculate the length of the stem
    size_t stem_length = ext_pos ? (size_t)(ext_pos - filename) : strlen(filename);

    // Copy the stem to the output buffer safely
    if (stem_length >= 255) {
        fprintf(stderr, "Buffer too small for stem\n");
        return;
    }

    strncpy(stem, filename, stem_length);
    stem[stem_length] = '\0'; // Null-terminate the string
}

void cart_battery_load() {
    FILE *file = fopen(save, "rb");

    if (!file) {
        printf("Failed to open battery save\n");
        return;
    }

    fread(cart.ram, SRAM_BANK_SIZE, cart.num_ram_banks, file);
    fclose(file);
}

void cart_battery_save() {

    FILE *file = fopen(save, "wb");

    if (!file) {
        printf("Failed to open battery save\n");
        return;
    }

    fwrite(cart.ram, SRAM_BANK_SIZE, cart.num_ram_banks, file);
    fclose(file);

    printf("Save\n");
}
