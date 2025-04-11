#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct {
    uint8_t hi;
    uint8_t lo;
} u16_bytes;

// Read the nth bit of an 8-bit value
static inline bool bit_read(u8 x, u8 n) {
    return (x >> n) & 1;
}

// Return an 8-bit value equal to x with the nth bit set to 1
static inline u8 bit_set(u8 x, u8 n) {
    return x | (1 << n);
}

// Return an 8-bit value equal to x with the nth bit set to 0
static inline u8 bit_clear(u8 x, u8 n) {
    return x & ~(1 << n);
}

// Return an 8-bit value equal to x with the nth bit set to f
static inline u8 bit_assign(u8 x, u8 n, bool f) {
    return x ^ ((-f ^ x) & (1 << n));
}

static inline u16_bytes u16_to_bytes(u16 value) {
    u8 hi = (value >> 8) & 0xFF;
    u8 lo = value & 0xFF;
    return (u16_bytes){ hi, lo };
}

static inline u16 u16_from_bytes(u16_bytes bytes) {
    return (bytes.hi << 8) | bytes.lo;
}
