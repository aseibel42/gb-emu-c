#include <stdio.h>
#include "util.h"

int main(int argc, char **argv) {
    uint8_t a = 0x12;
    uint8_t b = 0x34;
    uint16_t x = u16_from_bytes((u16_bytes){ a, b });

    printf("%x %x %x\n", a, b, x);
    return 0;
}
