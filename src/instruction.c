#include "cpu.h"
#include "instruction.h"

/*
 * Machine control instructions
*/

void nop() {}  // 0x00
void halt() { halted = true; }  // 0x76
void stop() { stopped = true; }  // 0x10

/*
 * Interrupt instructions
*/

void di() { interrupt_master_enabled = false; }  // 0xF3

// The IME flag is only set after the instruction following EI
void ei() { interrupt_master_enabled = true; }  // 0xFB

// This is equivalent to executing EI then RET, meaning that IME is set right after this instruction.
void ret_i() { interrupt_master_enabled = true; ret(); }  // 0xD9

/*
 * Load instructions
 */

void ld_ab() { cpu.reg.a = cpu.reg.b; }  // 0x78
void ld_ac() { cpu.reg.a = cpu.reg.c; }  // 0x79
void ld_ad() { cpu.reg.a = cpu.reg.d; }  // 0x7A
void ld_ae() { cpu.reg.a = cpu.reg.e; }  // 0x7B
void ld_ah() { cpu.reg.a = cpu.reg.h; }  // 0x7C
void ld_al() { cpu.reg.a = cpu.reg.l; }  // 0x7D
void ld_aa() { cpu.reg.a = cpu.reg.a; }  // 0x7F
void ld_ax() { cpu.reg.a = cpu_read(cpu.reg.hl); }  // 0x7E
void ld_an() { cpu.reg.a = cpu_fetch(); }  // 0x3E

void ld_bb() { cpu.reg.b = cpu.reg.b; }  // 0x40
void ld_bc() { cpu.reg.b = cpu.reg.c; }  // 0x41
void ld_bd() { cpu.reg.b = cpu.reg.d; }  // 0x42
void ld_be() { cpu.reg.b = cpu.reg.e; }  // 0x43
void ld_bh() { cpu.reg.b = cpu.reg.h; }  // 0x44
void ld_bl() { cpu.reg.b = cpu.reg.l; }  // 0x45
void ld_ba() { cpu.reg.b = cpu.reg.a; }  // 0x47
void ld_bx() { cpu.reg.b = cpu_read(cpu.reg.hl); }  // 0x46
void ld_bn() { cpu.reg.b = cpu_fetch(); }  // 0x06

void ld_cb() { cpu.reg.c = cpu.reg.b; }  // 0x48
void ld_cc() { cpu.reg.c = cpu.reg.c; }  // 0x49
void ld_cd() { cpu.reg.c = cpu.reg.d; }  // 0x4A
void ld_ce() { cpu.reg.c = cpu.reg.e; }  // 0x4B
void ld_ch() { cpu.reg.c = cpu.reg.h; }  // 0x4C
void ld_cl() { cpu.reg.c = cpu.reg.l; }  // 0x4D
void ld_ca() { cpu.reg.c = cpu.reg.a; }  // 0x4F
void ld_cx() { cpu.reg.c = cpu_read(cpu.reg.hl); }  // 0x4E
void ld_cn() { cpu.reg.c = cpu_fetch(); }  // 0x0E

void ld_db() { cpu.reg.d = cpu.reg.b; }  // 0x50
void ld_dc() { cpu.reg.d = cpu.reg.c; }  // 0x51
void ld_dd() { cpu.reg.d = cpu.reg.d; }  // 0x52
void ld_de() { cpu.reg.d = cpu.reg.e; }  // 0x53
void ld_dh() { cpu.reg.d = cpu.reg.h; }  // 0x54
void ld_dl() { cpu.reg.d = cpu.reg.l; }  // 0x55
void ld_da() { cpu.reg.d = cpu.reg.a; }  // 0x57
void ld_dx() { cpu.reg.d = cpu_read(cpu.reg.hl); }  // 0x56
void ld_dn() { cpu.reg.d = cpu_fetch(); }  // 0x16

void ld_eb() { cpu.reg.e = cpu.reg.b; }  // 0x58
void ld_ec() { cpu.reg.e = cpu.reg.c; }  // 0x59
void ld_ed() { cpu.reg.e = cpu.reg.d; }  // 0x5A
void ld_ee() { cpu.reg.e = cpu.reg.e; }  // 0x5B
void ld_eh() { cpu.reg.e = cpu.reg.h; }  // 0x5C
void ld_el() { cpu.reg.e = cpu.reg.l; }  // 0x5D
void ld_ea() { cpu.reg.e = cpu.reg.a; }  // 0x5F
void ld_ex() { cpu.reg.e = cpu_read(cpu.reg.hl); }  // 0x5E
void ld_en() { cpu.reg.e = cpu_fetch(); }  // 0x1E

void ld_hb() { cpu.reg.h = cpu.reg.b; }  // 0x60
void ld_hc() { cpu.reg.h = cpu.reg.c; }  // 0x61
void ld_hd() { cpu.reg.h = cpu.reg.d; }  // 0x62
void ld_he() { cpu.reg.h = cpu.reg.e; }  // 0x63
void ld_hh() { cpu.reg.h = cpu.reg.h; }  // 0x64
void ld_hl() { cpu.reg.h = cpu.reg.l; }  // 0x65
void ld_ha() { cpu.reg.h = cpu.reg.a; }  // 0x67
void ld_hx() { cpu.reg.h = cpu_read(cpu.reg.hl); }  // 0x66
void ld_hn() { cpu.reg.h = cpu_fetch(); }  // 0x26

void ld_lb() { cpu.reg.l = cpu.reg.b; }  // 0x68
void ld_lc() { cpu.reg.l = cpu.reg.c; }  // 0x69
void ld_ld() { cpu.reg.l = cpu.reg.d; }  // 0x6A
void ld_le() { cpu.reg.l = cpu.reg.e; }  // 0x6B
void ld_lh() { cpu.reg.l = cpu.reg.h; }  // 0x6C
void ld_ll() { cpu.reg.l = cpu.reg.l; }  // 0x6D
void ld_la() { cpu.reg.l = cpu.reg.a; }  // 0x6F
void ld_lx() { cpu.reg.l = cpu_read(cpu.reg.hl); }  // 0x6E
void ld_ln() { cpu.reg.l = cpu_fetch(); }  // 0x2E

void ld_xb() { cpu_write(cpu.reg.hl, cpu.reg.b); }  // 0x70
void ld_xc() { cpu_write(cpu.reg.hl, cpu.reg.c); }  // 0x71
void ld_xd() { cpu_write(cpu.reg.hl, cpu.reg.d); }  // 0x72
void ld_xe() { cpu_write(cpu.reg.hl, cpu.reg.e); }  // 0x73
void ld_xh() { cpu_write(cpu.reg.hl, cpu.reg.h); }  // 0x74
void ld_xl() { cpu_write(cpu.reg.hl, cpu.reg.l); }  // 0x75
void ld_xa() { cpu_write(cpu.reg.hl, cpu.reg.a); }  // 0x77
void ld_xn() { cpu_write(cpu.reg.hl, cpu_fetch()); }  // 0x36

void ld_a_bc() { cpu.reg.a = cpu_read(cpu.reg.bc); }  // 0x0A
void ld_a_de() { cpu.reg.a = cpu_read(cpu.reg.de); }  // 0x1A

void ld_bc_a() { cpu_write(cpu.reg.bc, cpu.reg.a); }  // 0x02
void ld_de_a() { cpu_write(cpu.reg.de, cpu.reg.a); }  // 0x12

void ld_nn_a() { cpu_write(cpu_fetch16(), cpu.reg.a); }  // 0xEA
void ld_a_nn() { cpu.reg.a = cpu_read(cpu_fetch16()); }  // 0xFA

void ld_bc_nn() { cpu.reg.bc = cpu_fetch16(); }  // 0x01
void ld_de_nn() { cpu.reg.de = cpu_fetch16(); }  // 0x11
void ld_hl_nn() { cpu.reg.hl = cpu_fetch16(); }  // 0x21
void ld_sp_nn() { cpu.reg.sp = cpu_fetch16(); }  // 0x31

void ldio_c_a() { cpu_write(0xFF00 + cpu.reg.c, cpu.reg.a); }  // 0xE2
void ldio_a_c() { cpu.reg.a = cpu_read(0xFF00 + cpu.reg.c); }  // 0xF2
void ldio_n_a() { cpu_write(0xFF00 + cpu_fetch(), cpu.reg.a); }  // 0xE0
void ldio_a_n() { cpu.reg.a = cpu_read(0xFF00 + cpu_fetch()); }  // 0xF0

void ldd_ax() { cpu.reg.a = cpu_read(cpu.reg.hl--); }  // 0x3A
void ldd_xa() { cpu_write(cpu.reg.hl--, cpu.reg.a); }  // 0x32
void ldi_ax() { cpu.reg.a = cpu_read(cpu.reg.hl++); }  // 0x2A
void ldi_xa() { cpu_write(cpu.reg.hl++, cpu.reg.a); }  // 0x22

void ld_sp_hl() { cpu_cycle(); cpu.reg.sp = cpu.reg.hl; }  // 0xF9

void ld_hl_sp_n() {
    i8 x = cpu_fetch();
    cpu_cycle();
    cpu.reg.hl = cpu.reg.sp + x;
    bool z = false;
    bool n = false;
    bool h = (cpu.reg.sp & 0xF) + (x & 0xF) > 0xF;
    bool c = (cpu.reg.sp & 0xFF) + (x & 0xFF) > 0xFF;
    set_flags(z, n, h, c);
}  // 0xF8

void ld_nn_sp() { cpu_write16(cpu_fetch16(), cpu.reg.sp); }  // 0x08

// NOTE: push instructions have unexplained internal CPU cycle
void push_af() { cpu_cycle(); stack_push16(cpu.reg.af); }  // 0xF5
void push_bc() { cpu_cycle(); stack_push16(cpu.reg.bc); }  // 0xC5
void push_de() { cpu_cycle(); stack_push16(cpu.reg.de); }  // 0xD5
void push_hl() { cpu_cycle(); stack_push16(cpu.reg.hl); }  // 0xE5

void pop_af() { cpu.reg.af = stack_pop16() & 0xFFF0; }  // 0xF1
void pop_bc() { cpu.reg.bc = stack_pop16(); }  // 0xC1
void pop_de() { cpu.reg.de = stack_pop16(); }  // 0xD1
void pop_hl() { cpu.reg.hl = stack_pop16(); }  // 0xE1

/*
 * Arithmetic instructions
 */

u8 _add(u8 a, u8 b) {
    u8 result = a + b;
    bool z = !result;
    bool n = false;
    bool h = (a & 0xF) + (b & 0xF) > 0xF;
    bool c = ((u16)a + b) > 0xFF;
    set_flags(z, n, h, c);
    return result;
}

void add_ab() { cpu.reg.a = _add(cpu.reg.a, cpu.reg.b); }  // 0x80
void add_ac() { cpu.reg.a = _add(cpu.reg.a, cpu.reg.c); }  // 0x81
void add_ad() { cpu.reg.a = _add(cpu.reg.a, cpu.reg.d); }  // 0x82
void add_ae() { cpu.reg.a = _add(cpu.reg.a, cpu.reg.e); }  // 0x83
void add_ah() { cpu.reg.a = _add(cpu.reg.a, cpu.reg.h); }  // 0x84
void add_al() { cpu.reg.a = _add(cpu.reg.a, cpu.reg.l); }  // 0x85
void add_aa() { cpu.reg.a = _add(cpu.reg.a, cpu.reg.a); }  // 0x87
void add_ax() { cpu.reg.a = _add(cpu.reg.a, cpu_read(cpu.reg.hl)); }  // 0x86
void add_an() { cpu.reg.a = _add(cpu.reg.a, cpu_fetch()); }  // 0xC6

u8 _adc(u8 a, u8 b) {
    u8 result = a + b + cpu.flag.c;
    bool z = !result;
    bool n = false;
    bool h = (a & 0xF) + (b & 0xF) + cpu.flag.c > 0xF;
    bool c = ((u16)a + b + cpu.flag.c) > 0xFF;
    set_flags(z, n, h, c);
    return result;
}

void adc_ab() { cpu.reg.a = _adc(cpu.reg.a, cpu.reg.b); }  // 0x88
void adc_ac() { cpu.reg.a = _adc(cpu.reg.a, cpu.reg.c); }  // 0x89
void adc_ad() { cpu.reg.a = _adc(cpu.reg.a, cpu.reg.d); }  // 0x8A
void adc_ae() { cpu.reg.a = _adc(cpu.reg.a, cpu.reg.e); }  // 0x8B
void adc_ah() { cpu.reg.a = _adc(cpu.reg.a, cpu.reg.h); }  // 0x8C
void adc_al() { cpu.reg.a = _adc(cpu.reg.a, cpu.reg.l); }  // 0x8D
void adc_aa() { cpu.reg.a = _adc(cpu.reg.a, cpu.reg.a); }  // 0x8F
void adc_ax() { cpu.reg.a = _adc(cpu.reg.a, cpu_read(cpu.reg.hl)); }  // 0x8E
void adc_an() { cpu.reg.a = _adc(cpu.reg.a, cpu_fetch()); }  // 0xCE

u8 _sub(u8 a, u8 b) {
    bool z = a == b;
    bool n = true;
    bool h = (a & 0xF) < (b & 0xF);
    bool c = a < b;
    set_flags(z, n, h, c);
    return a - b;
}

void sub_ab() { cpu.reg.a = _sub(cpu.reg.a, cpu.reg.b); }  // 0x90
void sub_ac() { cpu.reg.a = _sub(cpu.reg.a, cpu.reg.c); }  // 0x91
void sub_ad() { cpu.reg.a = _sub(cpu.reg.a, cpu.reg.d); }  // 0x92
void sub_ae() { cpu.reg.a = _sub(cpu.reg.a, cpu.reg.e); }  // 0x93
void sub_ah() { cpu.reg.a = _sub(cpu.reg.a, cpu.reg.h); }  // 0x94
void sub_al() { cpu.reg.a = _sub(cpu.reg.a, cpu.reg.l); }  // 0x95
void sub_aa() { cpu.reg.a = _sub(cpu.reg.a, cpu.reg.a); }  // 0x97
void sub_ax() { cpu.reg.a = _sub(cpu.reg.a, cpu_read(cpu.reg.hl)); }  // 0x96
void sub_an() { cpu.reg.a = _sub(cpu.reg.a, cpu_fetch()); }  // 0xD6

u8 _sbc(u8 a, u8 b) {
    i8 result = a - b - cpu.flag.c;
    bool z = !result;
    bool n = true;
    bool h = (a & 0xF) - (b & 0xF) - cpu.flag.c < 0;
    bool c = (b + cpu.flag.c) > a;
    set_flags(z, n, h, c);
    return result;
}

void sbc_ab() { cpu.reg.a = _sbc(cpu.reg.a, cpu.reg.b); }  // 0x98
void sbc_ac() { cpu.reg.a = _sbc(cpu.reg.a, cpu.reg.c); }  // 0x99
void sbc_ad() { cpu.reg.a = _sbc(cpu.reg.a, cpu.reg.d); }  // 0x9A
void sbc_ae() { cpu.reg.a = _sbc(cpu.reg.a, cpu.reg.e); }  // 0x9B
void sbc_ah() { cpu.reg.a = _sbc(cpu.reg.a, cpu.reg.h); }  // 0x9C
void sbc_al() { cpu.reg.a = _sbc(cpu.reg.a, cpu.reg.l); }  // 0x9D
void sbc_aa() { cpu.reg.a = _sbc(cpu.reg.a, cpu.reg.a); }  // 0x9F
void sbc_ax() { cpu.reg.a = _sbc(cpu.reg.a, cpu_read(cpu.reg.hl)); }  // 0x9E
void sbc_an() { cpu.reg.a = _sbc(cpu.reg.a, cpu_fetch()); }  // 0xDE

u8 _and(u8 a, u8 b) {
    u8 result = a & b;
    bool z = !result;
    bool n = false;
    bool h = true;
    bool c = false;
    set_flags(z, n, h, c);
    return result;
}

void and_ab() { cpu.reg.a = _and(cpu.reg.a, cpu.reg.b); }  // 0xA0
void and_ac() { cpu.reg.a = _and(cpu.reg.a, cpu.reg.c); }  // 0xA1
void and_ad() { cpu.reg.a = _and(cpu.reg.a, cpu.reg.d); }  // 0xA2
void and_ae() { cpu.reg.a = _and(cpu.reg.a, cpu.reg.e); }  // 0xA3
void and_ah() { cpu.reg.a = _and(cpu.reg.a, cpu.reg.h); }  // 0xA4
void and_al() { cpu.reg.a = _and(cpu.reg.a, cpu.reg.l); }  // 0xA5
void and_aa() { cpu.reg.a = _and(cpu.reg.a, cpu.reg.a); }  // 0xA7
void and_ax() { cpu.reg.a = _and(cpu.reg.a, cpu_read(cpu.reg.hl)); }  // 0xA6
void and_an() { cpu.reg.a = _and(cpu.reg.a, cpu_fetch()); }  // 0xE6

u8 _xor(u8 a, u8 b) {
    u8 result = a ^ b;
    bool z = !result;
    bool n = false;
    bool h = false;
    bool c = false;
    set_flags(z, n, h, c);
    return result;
}

void xor_ab() { cpu.reg.a = _xor(cpu.reg.a, cpu.reg.b); }  // 0xA8
void xor_ac() { cpu.reg.a = _xor(cpu.reg.a, cpu.reg.c); }  // 0xA9
void xor_ad() { cpu.reg.a = _xor(cpu.reg.a, cpu.reg.d); }  // 0xAA
void xor_ae() { cpu.reg.a = _xor(cpu.reg.a, cpu.reg.e); }  // 0xAB
void xor_ah() { cpu.reg.a = _xor(cpu.reg.a, cpu.reg.h); }  // 0xAC
void xor_al() { cpu.reg.a = _xor(cpu.reg.a, cpu.reg.l); }  // 0xAD
void xor_aa() { cpu.reg.a = _xor(cpu.reg.a, cpu.reg.a); }  // 0xAF
void xor_ax() { cpu.reg.a = _xor(cpu.reg.a, cpu_read(cpu.reg.hl)); }  // 0xAE
void xor_an() { cpu.reg.a = _xor(cpu.reg.a, cpu_fetch()); }  // 0xEE

u8 _or(u8 a, u8 b) {
    u8 result = a | b;
    bool z = !result;
    bool n = false;
    bool h = false;
    bool c = false;
    set_flags(z, n, h, c);
    return result;
}

void or_ab() { cpu.reg.a = _or(cpu.reg.a, cpu.reg.b); }  // 0xB0
void or_ac() { cpu.reg.a = _or(cpu.reg.a, cpu.reg.c); }  // 0xB1
void or_ad() { cpu.reg.a = _or(cpu.reg.a, cpu.reg.d); }  // 0xB2
void or_ae() { cpu.reg.a = _or(cpu.reg.a, cpu.reg.e); }  // 0xB3
void or_ah() { cpu.reg.a = _or(cpu.reg.a, cpu.reg.h); }  // 0xB4
void or_al() { cpu.reg.a = _or(cpu.reg.a, cpu.reg.l); }  // 0xB5
void or_aa() { cpu.reg.a = _or(cpu.reg.a, cpu.reg.a); }  // 0xB7
void or_ax() { cpu.reg.a = _or(cpu.reg.a, cpu_read(cpu.reg.hl)); }  // 0xB6
void or_an() { cpu.reg.a = _or(cpu.reg.a, cpu_fetch()); }  // 0xF6

void _cmp(u8 a, u8 b) {
    bool z = a == b;
    bool n = true;
    bool h = (a & 0xF) < (b & 0xF);
    bool c = a < b;
    set_flags(z, n, h, c);
}

void cmp_ab() { _cmp(cpu.reg.a, cpu.reg.b); }  // 0xB8
void cmp_ac() { _cmp(cpu.reg.a, cpu.reg.c); }  // 0xB9
void cmp_ad() { _cmp(cpu.reg.a, cpu.reg.d); }  // 0xBA
void cmp_ae() { _cmp(cpu.reg.a, cpu.reg.e); }  // 0xBB
void cmp_ah() { _cmp(cpu.reg.a, cpu.reg.h); }  // 0xBC
void cmp_al() { _cmp(cpu.reg.a, cpu.reg.l); }  // 0xBD
void cmp_aa() { _cmp(cpu.reg.a, cpu.reg.a); }  // 0xBF
void cmp_ax() { _cmp(cpu.reg.a, cpu_read(cpu.reg.hl)); }  // 0xBE
void cmp_an() { _cmp(cpu.reg.a, cpu_fetch()); }  // 0xFE

u8 _inc(u8 x) {
    x++;
    bool z = !x;
    bool n = false;
    bool h = (x & 0xF) == 0;
    set_flags(z, n, h, -1);
    return x;
}

void inc_b() { cpu.reg.b = _inc(cpu.reg.b); }  // 0x04
void inc_c() { cpu.reg.c = _inc(cpu.reg.c); }  // 0x0C
void inc_d() { cpu.reg.d = _inc(cpu.reg.d); }  // 0x14
void inc_e() { cpu.reg.e = _inc(cpu.reg.e); }  // 0x1C
void inc_h() { cpu.reg.h = _inc(cpu.reg.h); }  // 0x24
void inc_l() { cpu.reg.l = _inc(cpu.reg.l); }  // 0x2C
void inc_a() { cpu.reg.a = _inc(cpu.reg.a); }  // 0x3C
void inc_x() { cpu_write(cpu.reg.hl, _inc(cpu_read(cpu.reg.hl))); }  // 0x34

u8 _dec(u8 x) {
    x--;
    bool z = !x;
    bool n = true;
    bool h = (x & 0xF) == 0xF;
    set_flags(z, n, h, -1);
    return x;
}

void dec_b() { cpu.reg.b = _dec(cpu.reg.b); }  // 0x05
void dec_c() { cpu.reg.c = _dec(cpu.reg.c); }  // 0x0D
void dec_d() { cpu.reg.d = _dec(cpu.reg.d); }  // 0x15
void dec_e() { cpu.reg.e = _dec(cpu.reg.e); }  // 0x1D
void dec_h() { cpu.reg.h = _dec(cpu.reg.h); }  // 0x25
void dec_l() { cpu.reg.l = _dec(cpu.reg.l); }  // 0x2D
void dec_a() { cpu.reg.a = _dec(cpu.reg.a); }  // 0x3D
void dec_x() { cpu_write(cpu.reg.hl, _dec(cpu_read(cpu.reg.hl))); }  // 0x35

u16 _add16(u16 a, u16 b) {
    u16 result = a + b;
    bool n = false;
    bool h = (a & 0xFFF) + (b & 0xFFF) > 0xFFF;
    bool c = (result < a) && (result < b);
    set_flags(-1, n, h, c);
    return result;
}

void add16_bc() { cpu_cycle(); cpu.reg.hl = _add16(cpu.reg.hl, cpu.reg.bc); }  // 0x09
void add16_de() { cpu_cycle(); cpu.reg.hl = _add16(cpu.reg.hl, cpu.reg.de); }  // 0x19
void add16_hl() { cpu_cycle(); cpu.reg.hl = _add16(cpu.reg.hl, cpu.reg.hl); }  // 0x29
void add16_sp() { cpu_cycle(); cpu.reg.hl = _add16(cpu.reg.hl, cpu.reg.sp); }  // 0x39

void add_sp_n() {
    i8 x = cpu_fetch();
    bool z = false;
    bool n = false;
    bool h = (cpu.reg.sp & 0xF) + (x & 0xF) > 0xF;
    bool c = (cpu.reg.sp & 0xFF) + (x & 0xFF) > 0xFF;
    set_flags(z, n, h, c);
    cpu_cycle();
    cpu_cycle();
    cpu.reg.sp += x;
}  // 0xE8

void inc16_bc() { cpu_cycle(); cpu.reg.bc++; }  // 0x03
void inc16_de() { cpu_cycle(); cpu.reg.de++; }  // 0x13
void inc16_hl() { cpu_cycle(); cpu.reg.hl++; }  // 0x23
void inc16_sp() { cpu_cycle(); cpu.reg.sp++; }  // 0x33

void dec16_bc() { cpu_cycle(); cpu.reg.bc--; }  // 0x0B
void dec16_de() { cpu_cycle(); cpu.reg.de--; }  // 0x1B
void dec16_hl() { cpu_cycle(); cpu.reg.hl--; }  // 0x2B
void dec16_sp() { cpu_cycle(); cpu.reg.sp--; }  // 0x3B

/*
 * Bitwise instructions
*/

void rotlc_a() {
    u8 c = cpu.reg.a >> 7;
    cpu.reg.a = (cpu.reg.a << 1) | c;
    set_flags(0, 0, 0, c);
}  // 0x07

void rotrc_a() {
    u8 c = cpu.reg.a & 1;
    cpu.reg.a = (cpu.reg.a >> 1) | (c << 7);
    set_flags(0, 0, 0, c);
}  // 0x0F

void rotl_a() {
    u8 c = cpu.reg.a >> 7;
    cpu.reg.a = (cpu.reg.a << 1) | cpu.flag.c;
    set_flags(0, 0, 0, c);
}  // 0x17

void rotr_a() {
    u8 c = cpu.reg.a & 1;
    cpu.reg.a = (cpu.reg.a >> 1) | (cpu.flag.c << 7);
    set_flags(0, 0, 0, c);
}  // 0x1F

void cmpl_a() {
    cpu.reg.a = ~cpu.reg.a;
    set_flags(-1, 1, 1, -1);
}  // 0x2F

void dec_adj_a() {
    u8 x = 0;
    bool c = false;

    if (cpu.flag.h || (!cpu.flag.n && (cpu.reg.a & 0xF) > 9)) {
        x |= 0x6;
    }

    if (cpu.flag.c || (!cpu.flag.n && cpu.reg.a > 0x99)) {
        x |= 0x60;
        c = true;
    }

    cpu.reg.a += cpu.flag.n ? -x : x;
    bool z = !cpu.reg.a;
    set_flags(z, -1, 0, c);
}  // 0x27

void ccf(void) { set_flags(-1, 0, 0, !cpu.flag.c); }  // 0x3F
void scf(void) { set_flags(-1, 0, 0, 1); }  // 0x37

/*
 * Control flow instructions
*/

void jmp() { u16 nn = cpu_fetch16(); cpu_jump(nn); }  // 0xC3
void jmp_z() { u16 nn = cpu_fetch16(); if (cpu.flag.z) cpu_jump(nn); }  // 0xCA
void jmp_c() { u16 nn = cpu_fetch16(); if (cpu.flag.c) cpu_jump(nn); }  // 0xDA
void jmp_nz() { u16 nn = cpu_fetch16(); if (!cpu.flag.z) cpu_jump(nn); }  // 0xC2
void jmp_nc() { u16 nn = cpu_fetch16(); if (!cpu.flag.c) cpu_jump(nn); }  // 0xD2

void jmpr() { i8 n = cpu_fetch(); cpu_jmpr(n); }  // 0x18
void jmpr_z() { i8 n = cpu_fetch(); if (cpu.flag.z) cpu_jmpr(n); }  // 0x28
void jmpr_c() { i8 n = cpu_fetch(); if (cpu.flag.c) cpu_jmpr(n); }  // 0x38
void jmpr_nz() { i8 n = cpu_fetch(); if (!cpu.flag.z) cpu_jmpr(n); }  // 0x20
void jmpr_nc() { i8 n = cpu_fetch(); if (!cpu.flag.c) cpu_jmpr(n); }  // 0x30

// NOTE: for some reason, this jump does not cost an extra CPU cycle like every other jump
void jmp_x() { cpu.reg.pc = cpu.reg.hl; }  // 0xE9

void call() { u16 nn = cpu_fetch16(); stack_push16(cpu.reg.pc); cpu_jump(nn); }  // 0xCD
void call_z() { u16 nn = cpu_fetch16(); if (cpu.flag.z) { stack_push16(cpu.reg.pc); cpu_jump(nn); } }  // 0xCC
void call_c() { u16 nn = cpu_fetch16(); if (cpu.flag.c) { stack_push16(cpu.reg.pc); cpu_jump(nn); } }  // 0xDC
void call_nz() { u16 nn = cpu_fetch16(); if (!cpu.flag.z) { stack_push16(cpu.reg.pc); cpu_jump(nn); } }  // 0xC4
void call_nc() { u16 nn = cpu_fetch16(); if (!cpu.flag.c) { stack_push16(cpu.reg.pc); cpu_jump(nn); } }  // 0xD4

// NOTE: conditional return has unexplained internal CPU cycle
void ret() { u16 nn = stack_pop16(); cpu_jump(nn); }  // 0xC9
void ret_z() { cpu_cycle(); if (cpu.flag.z) { u16 nn = stack_pop16(); cpu_jump(nn); } }  // 0xC8
void ret_c() { cpu_cycle(); if (cpu.flag.c) { u16 nn = stack_pop16(); cpu_jump(nn); } }  // 0xD8
void ret_nz() { cpu_cycle(); if (!cpu.flag.z) { u16 nn = stack_pop16(); cpu_jump(nn); } }  // 0xC0
void ret_nc() { cpu_cycle(); if (!cpu.flag.c) { u16 nn = stack_pop16(); cpu_jump(nn); } }  // 0xD0

void rst_00() { stack_push16(cpu.reg.pc); cpu_jump(0x0000); }  // 0xC7
void rst_08() { stack_push16(cpu.reg.pc); cpu_jump(0x0008); }  // 0xCF
void rst_10() { stack_push16(cpu.reg.pc); cpu_jump(0x0010); }  // 0xD7
void rst_18() { stack_push16(cpu.reg.pc); cpu_jump(0x0018); }  // 0xDF
void rst_20() { stack_push16(cpu.reg.pc); cpu_jump(0x0020); }  // 0xE7
void rst_28() { stack_push16(cpu.reg.pc); cpu_jump(0x0028); }  // 0xEF
void rst_30() { stack_push16(cpu.reg.pc); cpu_jump(0x0030); }  // 0xF7
void rst_38() { stack_push16(cpu.reg.pc); cpu_jump(0x0038); }  // 0xFF

/*
 * CB instructions
*/

void cb() { u8 code = cpu_fetch(); op_cb[code](); }  // 0xCB

u8 _rlc(u8 r) {
    u8 c = r & 0x80;
    r = (r << 1) | (c >> 7);
    set_flags(!r, 0, 0, c);
    return r;
}

void rlc_b() { cpu.reg.b = _rlc(cpu.reg.b); }  // CB 00
void rlc_c() { cpu.reg.c = _rlc(cpu.reg.c); }  // CB 01
void rlc_d() { cpu.reg.d = _rlc(cpu.reg.d); }  // CB 02
void rlc_e() { cpu.reg.e = _rlc(cpu.reg.e); }  // CB 03
void rlc_h() { cpu.reg.h = _rlc(cpu.reg.h); }  // CB 04
void rlc_l() { cpu.reg.l = _rlc(cpu.reg.l); }  // CB 05
void rlc_x() { cpu_write(cpu.reg.hl, _rlc(cpu_read(cpu.reg.hl))); }  // CB 06
void rlc_a() { cpu.reg.a = _rlc(cpu.reg.a); }  // CB 07

u8 _rrc(u8 r) {
    u8 c = r & 1;
    r = (r >> 1) | (c << 7);
    set_flags(!r, 0, 0, c);
    return r;
}

void rrc_b() { cpu.reg.b = _rrc(cpu.reg.b); }  // CB 08
void rrc_c() { cpu.reg.c = _rrc(cpu.reg.c); }  // CB 09
void rrc_d() { cpu.reg.d = _rrc(cpu.reg.d); }  // CB 0A
void rrc_e() { cpu.reg.e = _rrc(cpu.reg.e); }  // CB 0B
void rrc_h() { cpu.reg.h = _rrc(cpu.reg.h); }  // CB 0C
void rrc_l() { cpu.reg.l = _rrc(cpu.reg.l); }  // CB 0D
void rrc_x() { cpu_write(cpu.reg.hl, _rrc(cpu_read(cpu.reg.hl))); }  // CB 0E
void rrc_a() { cpu.reg.a = _rrc(cpu.reg.a); }  // CB 0F

u8 _rl(u8 r) {
    u8 c = r & 0x80;
    r = (r << 1) | cpu.flag.c;
    set_flags(!r, 0, 0, c);
    return r;
}

void rl_b() { cpu.reg.b = _rl(cpu.reg.b); }  // CB 10
void rl_c() { cpu.reg.c = _rl(cpu.reg.c); }  // CB 11
void rl_d() { cpu.reg.d = _rl(cpu.reg.d); }  // CB 12
void rl_e() { cpu.reg.e = _rl(cpu.reg.e); }  // CB 13
void rl_h() { cpu.reg.h = _rl(cpu.reg.h); }  // CB 14
void rl_l() { cpu.reg.l = _rl(cpu.reg.l); }  // CB 15
void rl_x() { cpu_write(cpu.reg.hl, _rl(cpu_read(cpu.reg.hl))); }  // CB 16
void rl_a() { cpu.reg.a = _rl(cpu.reg.a); }  // CB 17

u8 _rr(u8 r) {
    u8 c = r & 1;
    r = (r >> 1) | (cpu.flag.c << 7);
    set_flags(!r, 0, 0, c);
    return r;
}

void rr_b() { cpu.reg.b = _rr(cpu.reg.b); }  // CB 18
void rr_c() { cpu.reg.c = _rr(cpu.reg.c); }  // CB 19
void rr_d() { cpu.reg.d = _rr(cpu.reg.d); }  // CB 1A
void rr_e() { cpu.reg.e = _rr(cpu.reg.e); }  // CB 1B
void rr_h() { cpu.reg.h = _rr(cpu.reg.h); }  // CB 1C
void rr_l() { cpu.reg.l = _rr(cpu.reg.l); }  // CB 1D
void rr_x() { cpu_write(cpu.reg.hl, _rr(cpu_read(cpu.reg.hl))); }  // CB 1E
void rr_a() { cpu.reg.a = _rr(cpu.reg.a); }  // CB 1F

u8 _sla(u8 r) {
    u8 c = r & 0x80;
    r = r << 1;
    set_flags(!r, 0, 0, c);
    return r;
}

void sla_b() { cpu.reg.b = _sla(cpu.reg.b); }  // CB 20
void sla_c() { cpu.reg.c = _sla(cpu.reg.c); }  // CB 21
void sla_d() { cpu.reg.d = _sla(cpu.reg.d); }  // CB 22
void sla_e() { cpu.reg.e = _sla(cpu.reg.e); }  // CB 23
void sla_h() { cpu.reg.h = _sla(cpu.reg.h); }  // CB 24
void sla_l() { cpu.reg.l = _sla(cpu.reg.l); }  // CB 25
void sla_x() { cpu_write(cpu.reg.hl, _sla(cpu_read(cpu.reg.hl))); }  // CB 26
void sla_a() { cpu.reg.a = _sla(cpu.reg.a); }  // CB 27

u8 _sra(u8 r) {
    u8 c = r & 1;
    r = (r >> 1) | (r & 0x80);
    set_flags(!r, 0, 0, c);
    return r;
}

void sra_b() { cpu.reg.b = _sra(cpu.reg.b); }  // CB 28
void sra_c() { cpu.reg.c = _sra(cpu.reg.c); }  // CB 29
void sra_d() { cpu.reg.d = _sra(cpu.reg.d); }  // CB 2A
void sra_e() { cpu.reg.e = _sra(cpu.reg.e); }  // CB 2B
void sra_h() { cpu.reg.h = _sra(cpu.reg.h); }  // CB 2C
void sra_l() { cpu.reg.l = _sra(cpu.reg.l); }  // CB 2D
void sra_x() { cpu_write(cpu.reg.hl, _sra(cpu_read(cpu.reg.hl))); }  // CB 2E
void sra_a() { cpu.reg.a = _sra(cpu.reg.a); }  // CB 2F

u8 _swap(u8 r) {
    r = (r & 0x0F) << 4 | (r & 0xF0) >> 4;
    set_flags(!r, 0, 0, 0);
    return r;
}

void swap_b() { cpu.reg.b = _swap(cpu.reg.b); }  // CB 30
void swap_c() { cpu.reg.c = _swap(cpu.reg.c); }  // CB 31
void swap_d() { cpu.reg.d = _swap(cpu.reg.d); }  // CB 32
void swap_e() { cpu.reg.e = _swap(cpu.reg.e); }  // CB 33
void swap_h() { cpu.reg.h = _swap(cpu.reg.h); }  // CB 34
void swap_l() { cpu.reg.l = _swap(cpu.reg.l); }  // CB 35
void swap_x() { cpu_write(cpu.reg.hl, _swap(cpu_read(cpu.reg.hl))); }  // CB 36
void swap_a() { cpu.reg.a = _swap(cpu.reg.a); }  // CB 37

// SRL - shift register r right into carry (LSB -> c, MSB -> 0)
u8 _srl(u8 r) {
    u8 c = r & 1;
    r = (r >> 1) & ~(1 << 7);
    set_flags(!r, 0, 0, c);
    return r;
}

void srl_b() { cpu.reg.b = _srl(cpu.reg.b); }  // CB 38
void srl_c() { cpu.reg.c = _srl(cpu.reg.c); }  // CB 39
void srl_d() { cpu.reg.d = _srl(cpu.reg.d); }  // CB 3A
void srl_e() { cpu.reg.e = _srl(cpu.reg.e); }  // CB 3B
void srl_h() { cpu.reg.h = _srl(cpu.reg.h); }  // CB 3C
void srl_l() { cpu.reg.l = _srl(cpu.reg.l); }  // CB 3D
void srl_x() { cpu_write(cpu.reg.hl, _srl(cpu_read(cpu.reg.hl))); }  // CB 3E
void srl_a() { cpu.reg.a = _srl(cpu.reg.a); }  // CB 3F

void _bit(u8 r, u8 n) {
    bool z = !bit_read(r, n);
    set_flags(z, 0, 1, -1);
}

void bit0_b() { _bit(cpu.reg.b, 0); }  // CB 40
void bit0_c() { _bit(cpu.reg.c, 0); }  // CB 41
void bit0_d() { _bit(cpu.reg.d, 0); }  // CB 42
void bit0_e() { _bit(cpu.reg.e, 0); }  // CB 43
void bit0_h() { _bit(cpu.reg.h, 0); }  // CB 44
void bit0_l() { _bit(cpu.reg.l, 0); }  // CB 45
void bit0_x() { _bit(cpu_read(cpu.reg.hl), 0); }  // CB 46
void bit0_a() { _bit(cpu.reg.a, 0); }  // CB 47

void bit1_b() { _bit(cpu.reg.b, 1); }  // CB 48
void bit1_c() { _bit(cpu.reg.c, 1); }  // CB 49
void bit1_d() { _bit(cpu.reg.d, 1); }  // CB 4A
void bit1_e() { _bit(cpu.reg.e, 1); }  // CB 4B
void bit1_h() { _bit(cpu.reg.h, 1); }  // CB 4C
void bit1_l() { _bit(cpu.reg.l, 1); }  // CB 4D
void bit1_x() { _bit(cpu_read(cpu.reg.hl), 1); }  // CB 4E
void bit1_a() { _bit(cpu.reg.a, 1); }  // CB 4F

void bit2_b() { _bit(cpu.reg.b, 2); }  // CB 50
void bit2_c() { _bit(cpu.reg.c, 2); }  // CB 51
void bit2_d() { _bit(cpu.reg.d, 2); }  // CB 52
void bit2_e() { _bit(cpu.reg.e, 2); }  // CB 53
void bit2_h() { _bit(cpu.reg.h, 2); }  // CB 54
void bit2_l() { _bit(cpu.reg.l, 2); }  // CB 55
void bit2_x() { _bit(cpu_read(cpu.reg.hl), 2); }  // CB 56
void bit2_a() { _bit(cpu.reg.a, 2); }  // CB 57

void bit3_b() { _bit(cpu.reg.b, 3); }  // CB 58
void bit3_c() { _bit(cpu.reg.c, 3); }  // CB 59
void bit3_d() { _bit(cpu.reg.d, 3); }  // CB 5A
void bit3_e() { _bit(cpu.reg.e, 3); }  // CB 5B
void bit3_h() { _bit(cpu.reg.h, 3); }  // CB 5C
void bit3_l() { _bit(cpu.reg.l, 3); }  // CB 5D
void bit3_x() { _bit(cpu_read(cpu.reg.hl), 3); }  // CB 5E
void bit3_a() { _bit(cpu.reg.a, 3); }  // CB 5F

void bit4_b() { _bit(cpu.reg.b, 4); }  // CB 60
void bit4_c() { _bit(cpu.reg.c, 4); }  // CB 61
void bit4_d() { _bit(cpu.reg.d, 4); }  // CB 62
void bit4_e() { _bit(cpu.reg.e, 4); }  // CB 63
void bit4_h() { _bit(cpu.reg.h, 4); }  // CB 64
void bit4_l() { _bit(cpu.reg.l, 4); }  // CB 65
void bit4_x() { _bit(cpu_read(cpu.reg.hl), 4); }  // CB 66
void bit4_a() { _bit(cpu.reg.a, 4); }  // CB 67

void bit5_b() { _bit(cpu.reg.b, 5); }  // CB 68
void bit5_c() { _bit(cpu.reg.c, 5); }  // CB 69
void bit5_d() { _bit(cpu.reg.d, 5); }  // CB 6A
void bit5_e() { _bit(cpu.reg.e, 5); }  // CB 6B
void bit5_h() { _bit(cpu.reg.h, 5); }  // CB 6C
void bit5_l() { _bit(cpu.reg.l, 5); }  // CB 6D
void bit5_x() { _bit(cpu_read(cpu.reg.hl), 5); }  // CB 6E
void bit5_a() { _bit(cpu.reg.a, 5); }  // CB 6F

void bit6_b() { _bit(cpu.reg.b, 6); }  // CB 70
void bit6_c() { _bit(cpu.reg.c, 6); }  // CB 71
void bit6_d() { _bit(cpu.reg.d, 6); }  // CB 72
void bit6_e() { _bit(cpu.reg.e, 6); }  // CB 73
void bit6_h() { _bit(cpu.reg.h, 6); }  // CB 74
void bit6_l() { _bit(cpu.reg.l, 6); }  // CB 75
void bit6_x() { _bit(cpu_read(cpu.reg.hl), 6); }  // CB 76
void bit6_a() { _bit(cpu.reg.a, 6); }  // CB 77

void bit7_b() { _bit(cpu.reg.b, 7); }  // CB 78
void bit7_c() { _bit(cpu.reg.c, 7); }  // CB 79
void bit7_d() { _bit(cpu.reg.d, 7); }  // CB 7A
void bit7_e() { _bit(cpu.reg.e, 7); }  // CB 7B
void bit7_h() { _bit(cpu.reg.h, 7); }  // CB 7C
void bit7_l() { _bit(cpu.reg.l, 7); }  // CB 7D
void bit7_x() { _bit(cpu_read(cpu.reg.hl), 7); }  // CB 7E
void bit7_a() { _bit(cpu.reg.a, 7); }  // CB 7F

void res0_b() { cpu.reg.b = bit_clear(cpu.reg.b, 0); }  // CB 80
void res0_c() { cpu.reg.c = bit_clear(cpu.reg.c, 0); }  // CB 81
void res0_d() { cpu.reg.d = bit_clear(cpu.reg.d, 0); }  // CB 82
void res0_e() { cpu.reg.e = bit_clear(cpu.reg.e, 0); }  // CB 83
void res0_h() { cpu.reg.h = bit_clear(cpu.reg.h, 0); }  // CB 84
void res0_l() { cpu.reg.l = bit_clear(cpu.reg.l, 0); }  // CB 85
void res0_x() { cpu_write(cpu.reg.hl, bit_clear(cpu_read(cpu.reg.hl), 0)); }  // CB 86
void res0_a() { cpu.reg.a = bit_clear(cpu.reg.a, 0); }  // CB 87

void res1_b() { cpu.reg.b = bit_clear(cpu.reg.b, 1); }  // CB 88
void res1_c() { cpu.reg.c = bit_clear(cpu.reg.c, 1); }  // CB 89
void res1_d() { cpu.reg.d = bit_clear(cpu.reg.d, 1); }  // CB 8A
void res1_e() { cpu.reg.e = bit_clear(cpu.reg.e, 1); }  // CB 8B
void res1_h() { cpu.reg.h = bit_clear(cpu.reg.h, 1); }  // CB 8C
void res1_l() { cpu.reg.l = bit_clear(cpu.reg.l, 1); }  // CB 8D
void res1_x() { cpu_write(cpu.reg.hl, bit_clear(cpu_read(cpu.reg.hl), 1)); }  // CB 8E
void res1_a() { cpu.reg.a = bit_clear(cpu.reg.a, 1); }  // CB 8F

void res2_b() { cpu.reg.b = bit_clear(cpu.reg.b, 2); }  // CB 90
void res2_c() { cpu.reg.c = bit_clear(cpu.reg.c, 2); }  // CB 91
void res2_d() { cpu.reg.d = bit_clear(cpu.reg.d, 2); }  // CB 92
void res2_e() { cpu.reg.e = bit_clear(cpu.reg.e, 2); }  // CB 93
void res2_h() { cpu.reg.h = bit_clear(cpu.reg.h, 2); }  // CB 94
void res2_l() { cpu.reg.l = bit_clear(cpu.reg.l, 2); }  // CB 95
void res2_x() { cpu_write(cpu.reg.hl, bit_clear(cpu_read(cpu.reg.hl), 2)); }  // CB 96
void res2_a() { cpu.reg.a = bit_clear(cpu.reg.a, 2); }  // CB 97

void res3_b() { cpu.reg.b = bit_clear(cpu.reg.b, 3); }  // CB 98
void res3_c() { cpu.reg.c = bit_clear(cpu.reg.c, 3); }  // CB 99
void res3_d() { cpu.reg.d = bit_clear(cpu.reg.d, 3); }  // CB 9A
void res3_e() { cpu.reg.e = bit_clear(cpu.reg.e, 3); }  // CB 9B
void res3_h() { cpu.reg.h = bit_clear(cpu.reg.h, 3); }  // CB 9C
void res3_l() { cpu.reg.l = bit_clear(cpu.reg.l, 3); }  // CB 9D
void res3_x() { cpu_write(cpu.reg.hl, bit_clear(cpu_read(cpu.reg.hl), 3)); }  // CB 9E
void res3_a() { cpu.reg.a = bit_clear(cpu.reg.a, 3); }  // CB 9F

void res4_b() { cpu.reg.b = bit_clear(cpu.reg.b, 4); }  // CB A0
void res4_c() { cpu.reg.c = bit_clear(cpu.reg.c, 4); }  // CB A1
void res4_d() { cpu.reg.d = bit_clear(cpu.reg.d, 4); }  // CB A2
void res4_e() { cpu.reg.e = bit_clear(cpu.reg.e, 4); }  // CB A3
void res4_h() { cpu.reg.h = bit_clear(cpu.reg.h, 4); }  // CB A4
void res4_l() { cpu.reg.l = bit_clear(cpu.reg.l, 4); }  // CB A5
void res4_x() { cpu_write(cpu.reg.hl, bit_clear(cpu_read(cpu.reg.hl), 4)); }  // CB A6
void res4_a() { cpu.reg.a = bit_clear(cpu.reg.a, 4); }  // CB A7

void res5_b() { cpu.reg.b = bit_clear(cpu.reg.b, 5); }  // CB A8
void res5_c() { cpu.reg.c = bit_clear(cpu.reg.c, 5); }  // CB A9
void res5_d() { cpu.reg.d = bit_clear(cpu.reg.d, 5); }  // CB AA
void res5_e() { cpu.reg.e = bit_clear(cpu.reg.e, 5); }  // CB AB
void res5_h() { cpu.reg.h = bit_clear(cpu.reg.h, 5); }  // CB AC
void res5_l() { cpu.reg.l = bit_clear(cpu.reg.l, 5); }  // CB AD
void res5_x() { cpu_write(cpu.reg.hl, bit_clear(cpu_read(cpu.reg.hl), 5)); }  // CB AE
void res5_a() { cpu.reg.a = bit_clear(cpu.reg.a, 5); }  // CB AF

void res6_b() { cpu.reg.b = bit_clear(cpu.reg.b, 6); }  // CB B0
void res6_c() { cpu.reg.c = bit_clear(cpu.reg.c, 6); }  // CB B1
void res6_d() { cpu.reg.d = bit_clear(cpu.reg.d, 6); }  // CB B2
void res6_e() { cpu.reg.e = bit_clear(cpu.reg.e, 6); }  // CB B3
void res6_h() { cpu.reg.h = bit_clear(cpu.reg.h, 6); }  // CB B4
void res6_l() { cpu.reg.l = bit_clear(cpu.reg.l, 6); }  // CB B5
void res6_x() { cpu_write(cpu.reg.hl, bit_clear(cpu_read(cpu.reg.hl), 6)); }  // CB B6
void res6_a() { cpu.reg.a = bit_clear(cpu.reg.a, 6); }  // CB B7

void res7_b() { cpu.reg.b = bit_clear(cpu.reg.b, 7); }  // CB B8
void res7_c() { cpu.reg.c = bit_clear(cpu.reg.c, 7); }  // CB B9
void res7_d() { cpu.reg.d = bit_clear(cpu.reg.d, 7); }  // CB BA
void res7_e() { cpu.reg.e = bit_clear(cpu.reg.e, 7); }  // CB BB
void res7_h() { cpu.reg.h = bit_clear(cpu.reg.h, 7); }  // CB BC
void res7_l() { cpu.reg.l = bit_clear(cpu.reg.l, 7); }  // CB BD
void res7_x() { cpu_write(cpu.reg.hl, bit_clear(cpu_read(cpu.reg.hl), 7)); }  // CB BE
void res7_a() { cpu.reg.a = bit_clear(cpu.reg.a, 7); }  // CB BF

void set0_b() { cpu.reg.b = bit_set(cpu.reg.b, 0); }  // CB C0
void set0_c() { cpu.reg.c = bit_set(cpu.reg.c, 0); }  // CB C1
void set0_d() { cpu.reg.d = bit_set(cpu.reg.d, 0); }  // CB C2
void set0_e() { cpu.reg.e = bit_set(cpu.reg.e, 0); }  // CB C3
void set0_h() { cpu.reg.h = bit_set(cpu.reg.h, 0); }  // CB C4
void set0_l() { cpu.reg.l = bit_set(cpu.reg.l, 0); }  // CB C5
void set0_x() { cpu_write(cpu.reg.hl, bit_set(cpu_read(cpu.reg.hl), 0)); }  // CB C6
void set0_a() { cpu.reg.a = bit_set(cpu.reg.a, 0); }  // CB C7

void set1_b() { cpu.reg.b = bit_set(cpu.reg.b, 1); }  // CB C8
void set1_c() { cpu.reg.c = bit_set(cpu.reg.c, 1); }  // CB C9
void set1_d() { cpu.reg.d = bit_set(cpu.reg.d, 1); }  // CB CA
void set1_e() { cpu.reg.e = bit_set(cpu.reg.e, 1); }  // CB CB
void set1_h() { cpu.reg.h = bit_set(cpu.reg.h, 1); }  // CB CC
void set1_l() { cpu.reg.l = bit_set(cpu.reg.l, 1); }  // CB CD
void set1_x() { cpu_write(cpu.reg.hl, bit_set(cpu_read(cpu.reg.hl), 1)); }  // CB CE
void set1_a() { cpu.reg.a = bit_set(cpu.reg.a, 1); }  // CB CF

void set2_b() { cpu.reg.b = bit_set(cpu.reg.b, 2); }  // CB D0
void set2_c() { cpu.reg.c = bit_set(cpu.reg.c, 2); }  // CB D1
void set2_d() { cpu.reg.d = bit_set(cpu.reg.d, 2); }  // CB D2
void set2_e() { cpu.reg.e = bit_set(cpu.reg.e, 2); }  // CB D3
void set2_h() { cpu.reg.h = bit_set(cpu.reg.h, 2); }  // CB D4
void set2_l() { cpu.reg.l = bit_set(cpu.reg.l, 2); }  // CB D5
void set2_x() { cpu_write(cpu.reg.hl, bit_set(cpu_read(cpu.reg.hl), 2)); }  // CB D6
void set2_a() { cpu.reg.a = bit_set(cpu.reg.a, 2); }  // CB D7

void set3_b() { cpu.reg.b = bit_set(cpu.reg.b, 3); }  // CB D8
void set3_c() { cpu.reg.c = bit_set(cpu.reg.c, 3); }  // CB D9
void set3_d() { cpu.reg.d = bit_set(cpu.reg.d, 3); }  // CB DA
void set3_e() { cpu.reg.e = bit_set(cpu.reg.e, 3); }  // CB DB
void set3_h() { cpu.reg.h = bit_set(cpu.reg.h, 3); }  // CB DC
void set3_l() { cpu.reg.l = bit_set(cpu.reg.l, 3); }  // CB DD
void set3_x() { cpu_write(cpu.reg.hl, bit_set(cpu_read(cpu.reg.hl), 3)); }  // CB DE
void set3_a() { cpu.reg.a = bit_set(cpu.reg.a, 3); }  // CB DF

void set4_b() { cpu.reg.b = bit_set(cpu.reg.b, 4); }  // CB E0
void set4_c() { cpu.reg.c = bit_set(cpu.reg.c, 4); }  // CB E1
void set4_d() { cpu.reg.d = bit_set(cpu.reg.d, 4); }  // CB E2
void set4_e() { cpu.reg.e = bit_set(cpu.reg.e, 4); }  // CB E3
void set4_h() { cpu.reg.h = bit_set(cpu.reg.h, 4); }  // CB E4
void set4_l() { cpu.reg.l = bit_set(cpu.reg.l, 4); }  // CB E5
void set4_x() { cpu_write(cpu.reg.hl, bit_set(cpu_read(cpu.reg.hl), 4)); }  // CB E6
void set4_a() { cpu.reg.a = bit_set(cpu.reg.a, 4); }  // CB E7

void set5_b() { cpu.reg.b = bit_set(cpu.reg.b, 5); }  // CB E8
void set5_c() { cpu.reg.c = bit_set(cpu.reg.c, 5); }  // CB E9
void set5_d() { cpu.reg.d = bit_set(cpu.reg.d, 5); }  // CB EA
void set5_e() { cpu.reg.e = bit_set(cpu.reg.e, 5); }  // CB EB
void set5_h() { cpu.reg.h = bit_set(cpu.reg.h, 5); }  // CB EC
void set5_l() { cpu.reg.l = bit_set(cpu.reg.l, 5); }  // CB ED
void set5_x() { cpu_write(cpu.reg.hl, bit_set(cpu_read(cpu.reg.hl), 5)); }  // CB EE
void set5_a() { cpu.reg.a = bit_set(cpu.reg.a, 5); }  // CB EF

void set6_b() { cpu.reg.b = bit_set(cpu.reg.b, 6); }  // CB F0
void set6_c() { cpu.reg.c = bit_set(cpu.reg.c, 6); }  // CB F1
void set6_d() { cpu.reg.d = bit_set(cpu.reg.d, 6); }  // CB F2
void set6_e() { cpu.reg.e = bit_set(cpu.reg.e, 6); }  // CB F3
void set6_h() { cpu.reg.h = bit_set(cpu.reg.h, 6); }  // CB F4
void set6_l() { cpu.reg.l = bit_set(cpu.reg.l, 6); }  // CB F5
void set6_x() { cpu_write(cpu.reg.hl, bit_set(cpu_read(cpu.reg.hl), 6)); }  // CB F6
void set6_a() { cpu.reg.a = bit_set(cpu.reg.a, 6); }  // CB F7

void set7_b() { cpu.reg.b = bit_set(cpu.reg.b, 7); }  // CB F8
void set7_c() { cpu.reg.c = bit_set(cpu.reg.c, 7); }  // CB F9
void set7_d() { cpu.reg.d = bit_set(cpu.reg.d, 7); }  // CB FA
void set7_e() { cpu.reg.e = bit_set(cpu.reg.e, 7); }  // CB FB
void set7_h() { cpu.reg.h = bit_set(cpu.reg.h, 7); }  // CB FC
void set7_l() { cpu.reg.l = bit_set(cpu.reg.l, 7); }  // CB FD
void set7_x() { cpu_write(cpu.reg.hl, bit_set(cpu_read(cpu.reg.hl), 7)); }  // CB FE
void set7_a() { cpu.reg.a = bit_set(cpu.reg.a, 7); }  // CB FF
