#pragma once

#include "util.h"

union buttons {
    struct {
        u8 a: 1;
        u8 b: 1;
        u8 select: 1;
        u8 start: 1;
        u8 right: 1;
        u8 left: 1;
        u8 up: 1;
        u8 down: 1;
    };
    struct {
        u8 ctrl: 4;
        u8 dpad: 4;
    };
    u8 value;
};

// Flags of the P1/JOYP register
typedef union {
    struct {
        u8 : 4;
        u8 select_dpad: 1;
        u8 select_btns: 1;
    };
    u8 value;
} joypad;

// Flags of LCDC register
typedef union {
    struct {
        u8 bgw_enable : 1;
        u8 obj_enable : 1;
        u8 obj_height : 1;
        u8 bg_tile_map : 1;
        u8 bgw_tiles : 1;
        u8 win_enable : 1;
        u8 win_tile_map : 1;
        u8 lcd_enable : 1;
    };
    u8 value;
} lcd_control;

// Flags of the STAT register
// Bits 3-6 are conditions for the STAT interrupt
typedef union {
    struct {
        u8 ppu_mode : 2;
        u8 lcd_y_cmp : 1;
        u8 hblank_int : 1;
        u8 vblank_int : 1;
        u8 oam_int : 1;
        u8 lcd_y_int : 1;
    };
    u8 value;
} lcd_stat;

// Bits of ch1 sweep register
typedef union {
    struct {
        u8 step : 3;
        u8 dir : 1;
        u8 pace : 3;
        u8 : 1;
    };
    u8 value;
} ch_sweep;

// Flags of ch2 volume register
typedef union {
    struct {
        u8 pace : 3;
        u8 dir : 1;
        u8 init_vol : 4;
    };
    u8 value;
} ch_vol;

// Flags of ch4 freq register
typedef union {
    struct {
        u8 divider : 3;
        u8 width : 1;
        u8 shift : 4;
    };
    u8 value;
} ch_freq;

// Flags of ch2 ctrl register
typedef union {
    struct {
        u8 period : 3;
        u8 : 3;
        u8 length_enable : 1;
        u8 trigger : 1;
    };
    u8 value;
} ch_ctrl;

// Flags of audio volume register
typedef union {
    struct {
        u8 vol_right : 3;
        u8 vin_right : 1;
        u8 vol_left : 3;
        u8 vin_left : 1;
    };
    u8 value;
} audio_vol;

// Flags of audio pan register
typedef union {
    struct {
        u8 ch1_right : 1;
        u8 ch2_right : 1;
        u8 ch3_right : 1;
        u8 ch4_right : 1;
        u8 ch1_left : 1;
        u8 ch2_left : 1;
        u8 ch3_left : 1;
        u8 ch4_left : 1;
    };
    u8 value;
} audio_pan;

// Flags of audio ctrl register
typedef union {
    struct {
        u8 ch1_enable : 1;
        u8 ch2_enable : 1;
        u8 ch3_enable : 1;
        u8 ch4_enable : 1;
        u8 : 3;
        u8 master_enable : 1;
    };
    u8 value;
} audio_ctrl;

// Contains hardware registers and High RAM (hram)
// Padding added to match layout of 256-byte zero page (bus.page_0)
struct io {
    joypad joyp; // 0xFF00
    u8 serial_data; // 0xFF01
    u8 serial_ctrl; // 0xFF02
    u8 : 8; // padding

    // timer
    u8 div; // 0xFF04
    u8 tima; // 0xFF05
    u8 tma; // 0xFF06
    u8 tac; // 0xFF07
    u32 : 32; // padding
    u16 : 16; // padding
    u8 : 8; // padding

    // interrupt
    u8 if_reg; // 0xFF0F

    // audio
    ch_sweep ch1_sweep; // 0xFF10
    u8 ch1_len; // 0xFF11
    ch_vol ch1_vol; // 0xFF12
    u8 ch1_freq; // 0xFF13
    ch_ctrl ch1_ctrl; // 0xFF14
    u8 ch2_sweep; // 0xFF15 - Not used
    u8 ch2_len; // 0xFF16
    ch_vol ch2_vol; // 0xFF17
    u8 ch2_freq; // 0xFF18
    ch_ctrl ch2_ctrl; // 0xFF19
    u8 ch3_sweep; // 0xFF1a - Not used
    u8 ch3_len; // 0xFF1b
    u8 ch3_vol; // 0xFF1c
    u8 ch3_freq; // 0xFF1d
    ch_ctrl ch3_ctrl; // 0xFF1e
    u8 ch4_sweep; // 0xFF1f - Not used
    u8 ch4_len; // 0xFF20
    ch_vol ch4_vol; // 0xFF21
    ch_freq ch4_freq; // 0xFF22
    ch_ctrl ch4_ctrl; // 0xFF23
    audio_vol master_vol; // 0xFF24
    audio_pan master_pan; // 0xFF25
    audio_ctrl master_ctrl; // 0xFF26
    u8 audio_padding[9];

    u8 wave[0x10]; // 0xFF30

    // lcd
    lcd_control lcdc; // 0xFF40
    lcd_stat stat; // 0xFF41
    u8 scroll_y; // 0xFF42
    u8 scroll_x; // 0xFF43
    u8 lcd_y; // 0xFF44
    u8 lcd_y_compare; // 0xFF45
    u8 dma; // 0xFF46
    u8 bg_palette; // 0xFF47
    u8 obj_palette[2]; // 0xFF48-FF49
    u8 win_y; // 0xFF4A
    u8 win_x; // 0xFF4B

    u16 : 16; // padding
    u8 : 8; // padding

    u8 vram_bank; // 0xFF4F
    u8 disable_boot; // 0xFF50
    u8 hdma_src[2]; // 0xFF51
    u8 hdma_dest[2]; // 0xFF53
    u8 hdma; // 0xFF55

    u16 : 16; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding

    u8 bgpi; // 0xFF68
    u8 bgpd; // 0xFF69
    u8 obpi; // 0xFF6A
    u8 obpd; // 0xFF6B

    u8 priority_mode; // 0xFF6C
    u8 : 8; // padding;
    u16 : 16; // padding;

    u8 wram_bank; // 0xFF70

    u8 : 8; // padding
    u16 : 16; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding

    u8 hram[0x7F];
    u8 ie_reg;
};

extern union buttons btns;
extern struct io io;

void io_init();
