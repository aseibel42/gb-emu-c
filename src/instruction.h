#pragma once

#include "util.h"

typedef enum {
    OP_NOP,
} op_type;

typedef enum {
    ADDR_R8,
    ADDR_HLD,
    ADDR_HLI,
} addr_mode;

typedef enum {
    R_NONE,
} reg_type;

typedef enum {
    COND_NONE,
    COND_NZ,
    COND_Z,
    COND_NC,
    COND_C,
} cond_type;

typedef struct {
    op_type type;
    addr_mode mode;
    reg_type reg1;
    reg_type reg2;
    cond_type cond;
    u8 param;
} instruction;

extern instruction instructions[0x100];
