#include <stdio.h>
#include <stdlib.h>

#include "cart.h"
#include "mem.h"

Cart cart = {0, NULL, 0, NULL};

// RAM size lookup table
static const size_t ram_size_table[] = {
    0,          // 0x00:   0 (no RAM)
    0,          // 0x01:  -- (unused)
    8 * 1024,   // 0x02:   8 KB
    32 * 1024,  // 0x03:  32 KB
    128 * 1024, // 0x04: 128 KB
    64 * 1024,  // 0x05:  64 KB
};

void load_rom(const char *filename) {
    printf("Loading ROM: %s\n", filename);

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

    // validate ROM size
    if (header.rom_size > 8) {
        fprintf(stderr, "Unsupported ROM size code: 0x%02X\n", header.rom_size);
        goto close;
    }

    // allocate memory for ROM
    cart.rom_size = 32 << (header.rom_size + 10);
    cart.rom = malloc(cart.rom_size);
    if (!cart.rom) {
        perror("Failed to allocate memory for ROM\n");
        goto close;
    }

    // read ROM
    rewind(file);
    if (fread(cart.rom, 1, cart.rom_size, file) != cart.rom_size) {
        perror("Failed to read ROM data\n");
        goto cleanup_rom;
    }

    // validate RAM size
    if (header.ram_size == 1 || header.ram_size > 5) {
        fprintf(stderr, "Unsupported RAM size code: 0x%02X\n", header.ram_size);
        goto cleanup_rom;
    }

    // allocate memory for RAM
    cart.ram_size = ram_size_table[header.ram_size];
    if (cart.ram_size > 0) {
        cart.ram = malloc(cart.ram_size);
        if (!cart.ram) {
            perror("Failed to allocate memory for RAM\n");
            goto cleanup_ram;
        }
    }

    // everything has succeeded - hookup to bus
    bus.rom_0 = cart.rom;
    bus.rom_1 = cart.rom + 0x4000;
    if (cart.ram_size > 0 && cart.ram) {
        bus.sram = cart.ram;
    }
    return;

// NOTE: use of labels and goto statements is generally discouraged, but it
// _is_ useful for resource management like closing files and freeing memory
// when the process can fail at multiple different stages.
cleanup_ram:
    free(cart.ram);
    cart.ram = NULL;
    cart.ram_size = 0;
cleanup_rom:
    free(cart.rom);
    cart.rom = NULL;
    cart.rom_size = 0;
close:
    fclose(file);
}
