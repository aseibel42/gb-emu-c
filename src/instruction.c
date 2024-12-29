#include "cpu.h"
#include "mem.h"
#include "stack.h"
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

void ld_ab() { cpu_reg.a = cpu_reg.b; }  // 0x78
void ld_ac() { cpu_reg.a = cpu_reg.c; }  // 0x79
void ld_ad() { cpu_reg.a = cpu_reg.d; }  // 0x7A
void ld_ae() { cpu_reg.a = cpu_reg.e; }  // 0x7B
void ld_ah() { cpu_reg.a = cpu_reg.h; }  // 0x7C
void ld_al() { cpu_reg.a = cpu_reg.l; }  // 0x7D
void ld_aa() { cpu_reg.a = cpu_reg.a; }  // 0x7F
void ld_ax() { cpu_reg.a = mem_read(cpu_reg.hl); }  // 0x7E
void ld_an() { cpu_reg.a = fetch(); }  // 0x3E

void ld_bb() { cpu_reg.b = cpu_reg.b; }  // 0x40
void ld_bc() { cpu_reg.b = cpu_reg.c; }  // 0x41
void ld_bd() { cpu_reg.b = cpu_reg.d; }  // 0x42
void ld_be() { cpu_reg.b = cpu_reg.e; }  // 0x43
void ld_bh() { cpu_reg.b = cpu_reg.h; }  // 0x44
void ld_bl() { cpu_reg.b = cpu_reg.l; }  // 0x45
void ld_ba() { cpu_reg.b = cpu_reg.a; }  // 0x47
void ld_bx() { cpu_reg.b = mem_read(cpu_reg.hl); }  // 0x46
void ld_bn() { cpu_reg.b = fetch(); }  // 0x06

void ld_cb() { cpu_reg.c = cpu_reg.b; }  // 0x48
void ld_cc() { cpu_reg.c = cpu_reg.c; }  // 0x49
void ld_cd() { cpu_reg.c = cpu_reg.d; }  // 0x4A
void ld_ce() { cpu_reg.c = cpu_reg.e; }  // 0x4B
void ld_ch() { cpu_reg.c = cpu_reg.h; }  // 0x4C
void ld_cl() { cpu_reg.c = cpu_reg.l; }  // 0x4D
void ld_ca() { cpu_reg.c = cpu_reg.a; }  // 0x4F
void ld_cx() { cpu_reg.c = mem_read(cpu_reg.hl); }  // 0x4E
void ld_cn() { cpu_reg.c = fetch(); }  // 0x0E

void ld_db() { cpu_reg.d = cpu_reg.b; }  // 0x50
void ld_dc() { cpu_reg.d = cpu_reg.c; }  // 0x51
void ld_dd() { cpu_reg.d = cpu_reg.d; }  // 0x52
void ld_de() { cpu_reg.d = cpu_reg.e; }  // 0x53
void ld_dh() { cpu_reg.d = cpu_reg.h; }  // 0x54
void ld_dl() { cpu_reg.d = cpu_reg.l; }  // 0x55
void ld_da() { cpu_reg.d = cpu_reg.a; }  // 0x57
void ld_dx() { cpu_reg.d = mem_read(cpu_reg.hl); }  // 0x56
void ld_dn() { cpu_reg.d = fetch(); }  // 0x16

void ld_eb() { cpu_reg.e = cpu_reg.b; }  // 0x58
void ld_ec() { cpu_reg.e = cpu_reg.c; }  // 0x59
void ld_ed() { cpu_reg.e = cpu_reg.d; }  // 0x5A
void ld_ee() { cpu_reg.e = cpu_reg.e; }  // 0x5B
void ld_eh() { cpu_reg.e = cpu_reg.h; }  // 0x5C
void ld_el() { cpu_reg.e = cpu_reg.l; }  // 0x5D
void ld_ea() { cpu_reg.e = cpu_reg.a; }  // 0x5F
void ld_ex() { cpu_reg.e = mem_read(cpu_reg.hl); }  // 0x5E
void ld_en() { cpu_reg.e = fetch(); }  // 0x1E

void ld_hb() { cpu_reg.h = cpu_reg.b; }  // 0x60
void ld_hc() { cpu_reg.h = cpu_reg.c; }  // 0x61
void ld_hd() { cpu_reg.h = cpu_reg.d; }  // 0x62
void ld_he() { cpu_reg.h = cpu_reg.e; }  // 0x63
void ld_hh() { cpu_reg.h = cpu_reg.h; }  // 0x64
void ld_hl() { cpu_reg.h = cpu_reg.l; }  // 0x65
void ld_ha() { cpu_reg.h = cpu_reg.a; }  // 0x67
void ld_hx() { cpu_reg.h = mem_read(cpu_reg.hl); }  // 0x66
void ld_hn() { cpu_reg.h = fetch(); }  // 0x26

void ld_lb() { cpu_reg.l = cpu_reg.b; }  // 0x68
void ld_lc() { cpu_reg.l = cpu_reg.c; }  // 0x69
void ld_ld() { cpu_reg.l = cpu_reg.d; }  // 0x6A
void ld_le() { cpu_reg.l = cpu_reg.e; }  // 0x6B
void ld_lh() { cpu_reg.l = cpu_reg.h; }  // 0x6C
void ld_ll() { cpu_reg.l = cpu_reg.l; }  // 0x6D
void ld_la() { cpu_reg.l = cpu_reg.a; }  // 0x6F
void ld_lx() { cpu_reg.l = mem_read(cpu_reg.hl); }  // 0x6E
void ld_ln() { cpu_reg.l = fetch(); }  // 0x2E

void ld_xb() { mem_write(cpu_reg.hl, cpu_reg.b); }  // 0x70
void ld_xc() { mem_write(cpu_reg.hl, cpu_reg.c); }  // 0x71
void ld_xd() { mem_write(cpu_reg.hl, cpu_reg.d); }  // 0x72
void ld_xe() { mem_write(cpu_reg.hl, cpu_reg.e); }  // 0x73
void ld_xh() { mem_write(cpu_reg.hl, cpu_reg.h); }  // 0x74
void ld_xl() { mem_write(cpu_reg.hl, cpu_reg.l); }  // 0x75
void ld_xa() { mem_write(cpu_reg.hl, cpu_reg.a); }  // 0x77
void ld_xn() { mem_write(cpu_reg.hl, fetch()); }  // 0x36

void ld_a_bc() { cpu_reg.a = mem_read(cpu_reg.bc); }  // 0x0A
void ld_a_de() { cpu_reg.a = mem_read(cpu_reg.de); }  // 0x1A

void ld_bc_a() { mem_write(cpu_reg.bc, cpu_reg.a); }  // 0x02
void ld_de_a() { mem_write(cpu_reg.de, cpu_reg.a); }  // 0x12

void ld_nn_a() { mem_write(fetch16(), cpu_reg.a); }  // 0xEA
void ld_a_nn() { cpu_reg.a = mem_read(fetch16()); }  // 0xFA

void ld_bc_nn() { cpu_reg.bc = fetch16(); }  // 0x01
void ld_de_nn() { cpu_reg.de = fetch16(); }  // 0x11
void ld_hl_nn() { cpu_reg.hl = fetch16(); }  // 0x21
void ld_sp_nn() { cpu_reg.sp = fetch16(); }  // 0x31

void ldio_c_a() { cpu_reg.a = mem_read(0xFF00 + cpu_reg.c); }  // 0xE2
void ldio_a_c() { mem_write(0xFF00 + cpu_reg.c, cpu_reg.a); }  // 0xF2
void ldio_n_a() { cpu_reg.a = mem_read(0xFF00 + fetch()); }  // 0xE0
void ldio_a_n() { mem_write(0xFF00 + fetch(), cpu_reg.a); }  // 0xF0

void ldd_ax() { cpu_reg.a = mem_read(cpu_reg.hl--); }  // 0x3A
void ldd_xa() { mem_write(cpu_reg.hl--, cpu_reg.a); }  // 0x32
void ldi_ax() { cpu_reg.a = mem_read(cpu_reg.hl++); }  // 0x2A
void ldi_xa() { mem_write(cpu_reg.hl++, cpu_reg.a); }  // 0x22

void ld_sp_hl() { cpu_jump(cpu_reg.hl); }  // 0xF9

void ld_hl_sp_n() {
    i8 x = fetch();
    cpu_cycle();
    cpu_reg.hl = cpu_reg.sp + x;
    bool z = false;
    bool n = false;
    bool h = (cpu_reg.sp & 0xF) + (x & 0xF) > 0xF;
    bool c = (cpu_reg.sp & 0xFF) + (x & 0xFF) > 0xFF;
    set_flags(z, n, h, c);
}  // 0xF8

void ld_nn_sp() { mem_write16(fetch16(), cpu_reg.sp); }  // 0x08

// NOTE: push instructions have unexplained internal CPU cycle
void push_af() { cpu_cycle(); stack_push16(cpu_reg.af); }  // 0xF5
void push_bc() { cpu_cycle(); stack_push16(cpu_reg.bc); }  // 0xC5
void push_de() { cpu_cycle(); stack_push16(cpu_reg.de); }  // 0xD5
void push_hl() { cpu_cycle(); stack_push16(cpu_reg.hl); }  // 0xE5

void pop_af() { cpu_reg.af = stack_pop16() & 0x0FFF; }  // 0xF1
void pop_bc() { cpu_reg.bc = stack_pop16(); }  // 0xC1
void pop_de() { cpu_reg.de = stack_pop16(); }  // 0xD1
void pop_hl() { cpu_reg.hl = stack_pop16(); }  // 0xE1

/*
 * Arithmetic instructions
 */

u8 _add(u8 a, u8 b) {
    u16 result = a + b;
    bool z = !result;
    bool n = false;
    bool h = (a & 0xF) + (b & 0xF) > 0xF;
    bool c = result > 0xFF;
    set_flags(z, n, h, c);
    return result;
}

void add_ab() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.b); }  // 0x80
void add_ac() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.c); }  // 0x81
void add_ad() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.d); }  // 0x82
void add_ae() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.e); }  // 0x83
void add_ah() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.h); }  // 0x84
void add_al() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.l); }  // 0x85
void add_aa() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.a); }  // 0x87
void add_ax() { cpu_reg.a = _add(cpu_reg.a, mem_read(cpu_reg.hl)); }  // 0x86
void add_an() { cpu_reg.a = _add(cpu_reg.a, fetch()); }  // 0xC6

u8 _adc(u8 a, u8 b) {
    u16 result = a + b + flag_c();
    bool z = !result;
    bool n = false;
    bool h = (a & 0xF) + (b & 0xF) + flag_c() > 0xF;
    bool c = result > 0xFF;
    set_flags(z, n, h, c);
    return result;
}

void adc_ab() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.b); }  // 0x88
void adc_ac() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.c); }  // 0x89
void adc_ad() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.d); }  // 0x8A
void adc_ae() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.e); }  // 0x8B
void adc_ah() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.h); }  // 0x8C
void adc_al() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.l); }  // 0x8D
void adc_aa() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.a); }  // 0x8F
void adc_ax() { cpu_reg.a = _adc(cpu_reg.a, mem_read(cpu_reg.hl)); }  // 0x8E
void adc_an() { cpu_reg.a = _adc(cpu_reg.a, fetch()); }  // 0xCE

u8 _sub(i8 a, i8 b) {
    i8 result = a - b;
    bool z = !result;
    bool n = true;
    bool h = (a & 0xF) - (b & 0xF) < 0;
    bool c = result < 0;
    set_flags(z, n, h, c);
    return result;
}

void sub_ab() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.b); }  // 0x90
void sub_ac() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.c); }  // 0x91
void sub_ad() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.d); }  // 0x92
void sub_ae() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.e); }  // 0x93
void sub_ah() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.h); }  // 0x94
void sub_al() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.l); }  // 0x95
void sub_aa() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.a); }  // 0x97
void sub_ax() { cpu_reg.a = _sub(cpu_reg.a, mem_read(cpu_reg.hl)); }  // 0x96
void sub_an() { cpu_reg.a = _sub(cpu_reg.a, fetch()); }  // 0xD6

u8 _sbc(i8 a, i8 b) {
    i8 result = a - b - flag_c();
    bool z = !result;
    bool n = true;
    bool h = (a & 0xF) - (b & 0xF) - flag_c() < 0;
    bool c = result < 0;
    set_flags(z, n, h, c);
    return result;
}

void sbc_ab() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.b); }  // 0x98
void sbc_ac() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.c); }  // 0x99
void sbc_ad() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.d); }  // 0x9A
void sbc_ae() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.e); }  // 0x9B
void sbc_ah() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.h); }  // 0x9C
void sbc_al() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.l); }  // 0x9D
void sbc_aa() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.a); }  // 0x9F
void sbc_ax() { cpu_reg.a = _sbc(cpu_reg.a, mem_read(cpu_reg.hl)); }  // 0x9E
void sbc_an() { cpu_reg.a = _sbc(cpu_reg.a, fetch()); }  // 0xDE

u8 _and(u8 a, u8 b) {
    u8 result = a & b;
    bool z = !result;
    bool n = false;
    bool h = true;
    bool c = false;
    set_flags(z, n, h, c);
    return result;
}

void and_ab() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.b); }  // 0xA0
void and_ac() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.c); }  // 0xA1
void and_ad() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.d); }  // 0xA2
void and_ae() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.e); }  // 0xA3
void and_ah() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.h); }  // 0xA4
void and_al() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.l); }  // 0xA5
void and_aa() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.a); }  // 0xA7
void and_ax() { cpu_reg.a = _and(cpu_reg.a, mem_read(cpu_reg.hl)); }  // 0xA6
void and_an() { cpu_reg.a = _and(cpu_reg.a, fetch()); }  // 0xE6

u8 _xor(u8 a, u8 b) {
    u8 result = a ^ b;
    bool z = !result;
    bool n = false;
    bool h = false;
    bool c = false;
    set_flags(z, n, h, c);
    return result;
}

void xor_ab() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.b); }  // 0xA8
void xor_ac() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.c); }  // 0xA9
void xor_ad() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.d); }  // 0xAA
void xor_ae() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.e); }  // 0xAB
void xor_ah() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.h); }  // 0xAC
void xor_al() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.l); }  // 0xAD
void xor_aa() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.a); }  // 0xAF
void xor_ax() { cpu_reg.a = _xor(cpu_reg.a, mem_read(cpu_reg.hl)); }  // 0xAE
void xor_an() { cpu_reg.a = _xor(cpu_reg.a, fetch()); }  // 0xEE

u8 _or(u8 a, u8 b) {
    u8 result = a | b;
    bool z = !result;
    bool n = false;
    bool h = false;
    bool c = false;
    set_flags(z, n, h, c);
    return result;
}

void or_ab() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.b); }  // 0xB0
void or_ac() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.c); }  // 0xB1
void or_ad() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.d); }  // 0xB2
void or_ae() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.e); }  // 0xB3
void or_ah() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.h); }  // 0xB4
void or_al() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.l); }  // 0xB5
void or_aa() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.a); }  // 0xB7
void or_ax() { cpu_reg.a = _or(cpu_reg.a, mem_read(cpu_reg.hl)); }  // 0xB6
void or_an() { cpu_reg.a = _or(cpu_reg.a, fetch()); }  // 0xF6

u8 _cmp(i8 a, i8 b) {
    i8 result = a - b;
    bool z = !result;
    bool n = true;
    bool h = (a & 0xF) - (b & 0xF) < 0;
    bool c = result < 0;
    set_flags(z, n, h, c);
    return result;
}

void cmp_ab() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.b); }  // 0xB8
void cmp_ac() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.c); }  // 0xB9
void cmp_ad() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.d); }  // 0xBA
void cmp_ae() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.e); }  // 0xBB
void cmp_ah() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.h); }  // 0xBC
void cmp_al() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.l); }  // 0xBD
void cmp_aa() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.a); }  // 0xBF
void cmp_ax() { cpu_reg.a = _cmp(cpu_reg.a, mem_read(cpu_reg.hl)); }  // 0xBE
void cmp_an() { cpu_reg.a = _cmp(cpu_reg.a, fetch()); }  // 0xFE

u8 _inc(u8 x) {
    x++;
    bool z = !x;
    bool n = false;
    bool h = (x & 0xF) == 0;
    set_flags(z, n, h, -1);
    return x;
}

void inc_b() { cpu_reg.b = _inc(cpu_reg.b); }  // 0x04
void inc_c() { cpu_reg.c = _inc(cpu_reg.c); }  // 0x0C
void inc_d() { cpu_reg.d = _inc(cpu_reg.d); }  // 0x14
void inc_e() { cpu_reg.e = _inc(cpu_reg.e); }  // 0x1C
void inc_h() { cpu_reg.h = _inc(cpu_reg.h); }  // 0x24
void inc_l() { cpu_reg.l = _inc(cpu_reg.l); }  // 0x2C
void inc_a() { cpu_reg.a = _inc(cpu_reg.a); }  // 0x3C
void inc_x() { mem_write(cpu_reg.hl, _inc(mem_read(cpu_reg.hl))); }  // 0x34

u8 _dec(u8 x) {
    x--;
    bool z = !x;
    bool n = true;
    bool h = (x & 0xF) == 0xF;
    set_flags(z, n, h, -1);
    return x;
}

void dec_b() { cpu_reg.b = _dec(cpu_reg.b); }  // 0x05
void dec_c() { cpu_reg.c = _dec(cpu_reg.c); }  // 0x0D
void dec_d() { cpu_reg.d = _dec(cpu_reg.d); }  // 0x15
void dec_e() { cpu_reg.e = _dec(cpu_reg.e); }  // 0x1D
void dec_h() { cpu_reg.h = _dec(cpu_reg.h); }  // 0x25
void dec_l() { cpu_reg.l = _dec(cpu_reg.l); }  // 0x2D
void dec_a() { cpu_reg.a = _dec(cpu_reg.a); }  // 0x3D
void dec_x() { mem_write(cpu_reg.hl, _dec(mem_read(cpu_reg.hl))); }  // 0x35

u16 _add16(u16 a, u16 b) {
    u16 result = a + b;
    bool n = false;
    bool h = (a & 0xFFF) + (b & 0xFFF) > 0xFFF;
    bool c = (result < a) && (result < b);
    set_flags(-1, n, h, c);
    return result;
}

void add16_bc() { cpu_cycle(); cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.bc); }  // 0x09
void add16_de() { cpu_cycle(); cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.de); }  // 0x19
void add16_hl() { cpu_cycle(); cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.hl); }  // 0x29
void add16_sp() { cpu_cycle(); cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.sp); }  // 0x39

void add_sp_n() {
    i8 x = fetch();
    bool z = false;
    bool n = false;
    bool h = (cpu_reg.sp & 0xF) + (x & 0xF) > 0xF;
    bool c = (cpu_reg.sp & 0xFF) + (x & 0xFF) > 0xFF;
    set_flags(z, n, h, c);
    cpu_cycle(); // 16-bit add incurs an extra cpu cycle
    cpu_jmpr(x);
}  // 0xE8

void inc16_bc() { cpu_cycle(); cpu_reg.bc++; }  // 0x03
void inc16_de() { cpu_cycle(); cpu_reg.de++; }  // 0x13
void inc16_hl() { cpu_cycle(); cpu_reg.hl++; }  // 0x23
void inc16_sp() { cpu_cycle(); cpu_reg.sp++; }  // 0x33

void dec16_bc() { cpu_cycle(); cpu_reg.bc--; }  // 0x0B
void dec16_de() { cpu_cycle(); cpu_reg.de--; }  // 0x1B
void dec16_hl() { cpu_cycle(); cpu_reg.hl--; }  // 0x2B
void dec16_sp() { cpu_cycle(); cpu_reg.sp--; }  // 0x3B

/*
 * Bitwise instructions
*/

void rotlc_a() {
    u8 c = cpu_reg.a >> 7;
    cpu_reg.a = (cpu_reg.a << 1) | c;
    bool z = !cpu_reg.a;
    set_flags(z, 0, 0, c);
}  // 0x07

void rotrc_a() {
    u8 c = cpu_reg.a & 1;
    cpu_reg.a = (cpu_reg.a >> 1) | (c << 7);
    bool z = !cpu_reg.a;
    set_flags(z, 0, 0, c);
}  // 0x0F

void rotl_a() {
    u8 c = cpu_reg.a >> 7;
    cpu_reg.a = (cpu_reg.a << 1) | flag_c();
    bool z = !cpu_reg.a;
    set_flags(z, 0, 0, c);
}  // 0x17

void rotr_a() {
    u8 c = cpu_reg.a & 1;
    cpu_reg.a = (cpu_reg.a >> 1) | (flag_c() << 7);
    bool z = !cpu_reg.a;
    set_flags(z, 0, 0, c);
}  // 0x1F

void cmpl_a() {
    cpu_reg.a = ~cpu_reg.a;
    set_flags(-1, 1, 1, -1);
}  // 0x2F

void dec_adj_a() {
    u8 x = 0;
    bool c = false;

    if (flag_h() || (!flag_n() && (cpu_reg.a & 0xF) > 9)) {
        x |= 0x6;
    }

    if (flag_c() || (!flag_n() && cpu_reg.a > 0x99)) {
        x |= 0x60;
        c = true;
    }

    cpu_reg.a += flag_n() ? -x : x;
    bool z = !cpu_reg.a;
    set_flags(z, -1, 0, c);
}  // 0x27

void ccf(void) { set_flags(-1, 0, 0, !flag_c()); }  // 0x3F
void scf(void) { set_flags(-1, 0, 0, 1); }  // 0x37

/*
 * Control flow instructions
*/

void jmp() { u16 nn = fetch16(); cpu_jump(nn); }  // 0xC3
void jmp_z() { u16 nn = fetch16(); if (flag_z()) cpu_jump(nn); }  // 0xCA
void jmp_c() { u16 nn = fetch16(); if (flag_c()) cpu_jump(nn); }  // 0xDA
void jmp_nz() { u16 nn = fetch16(); if (!flag_z()) cpu_jump(nn); }  // 0xC2
void jmp_nc() { u16 nn = fetch16(); if (!flag_c()) cpu_jump(nn); }  // 0xD2

void jmpr() { i8 n = fetch(); cpu_jmpr(n); }  // 0x18
void jmpr_z() { i8 n = fetch(); if (flag_z()) cpu_jmpr(n); }  // 0x28
void jmpr_c() { i8 n = fetch(); if (flag_c()) cpu_jmpr(n); }  // 0x38
void jmpr_nz() { i8 n = fetch(); if (!flag_z()) cpu_jmpr(n); }  // 0x20
void jmpr_nc() { i8 n = fetch(); if (!flag_c()) cpu_jmpr(n); }  // 0x30

// NOTE: for some reason, this jump does not cost an extra CPU cycle like every other jump
void jmp_x() { cpu_reg.pc = cpu_reg.hl; }  // 0xE9

void call() { u16 nn = fetch16(); stack_push16(cpu_reg.pc); cpu_jump(nn); }  // 0xCD
void call_z() { u16 nn = fetch16(); if (flag_z()) { stack_push16(cpu_reg.pc); cpu_jump(nn); } }  // 0xCC
void call_c() { u16 nn = fetch16(); if (flag_c()) { stack_push16(cpu_reg.pc); cpu_jump(nn); } }  // 0xDC
void call_nz() { u16 nn = fetch16(); if (!flag_z()) { stack_push16(cpu_reg.pc); cpu_jump(nn); } }  // 0xC4
void call_nc() { u16 nn = fetch16(); if (!flag_c()) { stack_push16(cpu_reg.pc); cpu_jump(nn); } }  // 0xD4

// NOTE: conditional return has unexplained internal CPU cycle
void ret() { u16 nn = stack_pop16(); cpu_jump(nn); }  // 0xC9
void ret_z() { cpu_cycle(); if (flag_z()) { u16 nn = stack_pop16(); cpu_jump(nn); } }  // 0xC8
void ret_c() { cpu_cycle(); if (flag_c()) { u16 nn = stack_pop16(); cpu_jump(nn); } }  // 0xD8
void ret_nz() { cpu_cycle(); if (!flag_z()) { u16 nn = stack_pop16(); cpu_jump(nn); } }  // 0xC0
void ret_nc() { cpu_cycle(); if (!flag_c()) { u16 nn = stack_pop16(); cpu_jump(nn); } }  // 0xD0

void rst_00() { stack_push16(cpu_reg.pc); cpu_jump(0x0000); }  // 0xC7
void rst_08() { stack_push16(cpu_reg.pc); cpu_jump(0x0008); }  // 0xCF
void rst_10() { stack_push16(cpu_reg.pc); cpu_jump(0x0010); }  // 0xD7
void rst_18() { stack_push16(cpu_reg.pc); cpu_jump(0x0018); }  // 0xDF
void rst_20() { stack_push16(cpu_reg.pc); cpu_jump(0x0020); }  // 0xE7
void rst_28() { stack_push16(cpu_reg.pc); cpu_jump(0x0028); }  // 0xEF
void rst_30() { stack_push16(cpu_reg.pc); cpu_jump(0x0030); }  // 0xF7
void rst_38() { stack_push16(cpu_reg.pc); cpu_jump(0x0038); }  // 0xFF

/*
 * CB instructions
*/

void cb() { u8 code = fetch(); op_cb[code](); }  // 0xCB

u8 _rlc(u8 r) {
    u8 c = r & 0x80;
    r = (r << 1) | (c >> 7);
    set_flags(!r, 0, 0, c);
    return r;
}

void rlc_b() { cpu_reg.b = _rlc(cpu_reg.b); }  // CB 00
void rlc_c() { cpu_reg.c = _rlc(cpu_reg.c); }  // CB 01
void rlc_d() { cpu_reg.d = _rlc(cpu_reg.d); }  // CB 02
void rlc_e() { cpu_reg.e = _rlc(cpu_reg.e); }  // CB 03
void rlc_h() { cpu_reg.h = _rlc(cpu_reg.h); }  // CB 04
void rlc_l() { cpu_reg.l = _rlc(cpu_reg.l); }  // CB 05
void rlc_x() { mem_write(cpu_reg.hl, _rlc(mem_read(cpu_reg.hl))); }  // CB 06
void rlc_a() { cpu_reg.a = _rlc(cpu_reg.a); }  // CB 07

u8 _rrc(u8 r) {
    u8 c = r & 1;
    r = (r >> 1) | (c << 7);
    set_flags(!r, 0, 0, c);
    return r;
}

void rrc_b() { cpu_reg.b = _rrc(cpu_reg.b); }  // CB 08
void rrc_c() { cpu_reg.c = _rrc(cpu_reg.c); }  // CB 09
void rrc_d() { cpu_reg.d = _rrc(cpu_reg.d); }  // CB 0A
void rrc_e() { cpu_reg.e = _rrc(cpu_reg.e); }  // CB 0B
void rrc_h() { cpu_reg.h = _rrc(cpu_reg.h); }  // CB 0C
void rrc_l() { cpu_reg.l = _rrc(cpu_reg.l); }  // CB 0D
void rrc_x() { mem_write(cpu_reg.hl, _rrc(mem_read(cpu_reg.hl))); }  // CB 0E
void rrc_a() { cpu_reg.a = _rrc(cpu_reg.a); }  // CB 0F

u8 _rl(u8 r) {
    u8 c = r & 0x80;
    r = (r << 1) | flag_c();
    set_flags(!r, 0, 0, c);
    return r;
}

void rl_b() { cpu_reg.b = _rl(cpu_reg.b); }  // CB 10
void rl_c() { cpu_reg.c = _rl(cpu_reg.c); }  // CB 11
void rl_d() { cpu_reg.d = _rl(cpu_reg.d); }  // CB 12
void rl_e() { cpu_reg.e = _rl(cpu_reg.e); }  // CB 13
void rl_h() { cpu_reg.h = _rl(cpu_reg.h); }  // CB 14
void rl_l() { cpu_reg.l = _rl(cpu_reg.l); }  // CB 15
void rl_x() { mem_write(cpu_reg.hl, _rl(mem_read(cpu_reg.hl))); }  // CB 16
void rl_a() { cpu_reg.a = _rl(cpu_reg.a); }  // CB 17

u8 _rr(u8 r) {
    u8 c = r & 1;
    r = (r >> 1) | (flag_c() << 7);
    set_flags(!r, 0, 0, c);
    return r;
}

void rr_b() { cpu_reg.b = _rr(cpu_reg.b); }  // CB 18
void rr_c() { cpu_reg.c = _rr(cpu_reg.c); }  // CB 19
void rr_d() { cpu_reg.d = _rr(cpu_reg.d); }  // CB 1A
void rr_e() { cpu_reg.e = _rr(cpu_reg.e); }  // CB 1B
void rr_h() { cpu_reg.h = _rr(cpu_reg.h); }  // CB 1C
void rr_l() { cpu_reg.l = _rr(cpu_reg.l); }  // CB 1D
void rr_x() { mem_write(cpu_reg.hl, _rr(mem_read(cpu_reg.hl))); }  // CB 1E
void rr_a() { cpu_reg.a = _rr(cpu_reg.a); }  // CB 1F

u8 _sla(u8 r) {
    u8 c = r & 0x08;
    r = (r << 1) | (r & 1);
    set_flags(!r, 0, 0, c);
    return r;
}

void sla_b() { cpu_reg.b = _sla(cpu_reg.b); }  // CB 20
void sla_c() { cpu_reg.c = _sla(cpu_reg.c); }  // CB 21
void sla_d() { cpu_reg.d = _sla(cpu_reg.d); }  // CB 22
void sla_e() { cpu_reg.e = _sla(cpu_reg.e); }  // CB 23
void sla_h() { cpu_reg.h = _sla(cpu_reg.h); }  // CB 24
void sla_l() { cpu_reg.l = _sla(cpu_reg.l); }  // CB 25
void sla_x() { mem_write(cpu_reg.hl, _sla(mem_read(cpu_reg.hl))); }  // CB 26
void sla_a() { cpu_reg.a = _sla(cpu_reg.a); }  // CB 27

u8 _sra(u8 r) {
    u8 c = r & 1;
    r = (r >> 1) | (r & 0x80);
    set_flags(!r, 0, 0, c);
    return r;
}

void sra_b() { cpu_reg.b = _sra(cpu_reg.b); }  // CB 28
void sra_c() { cpu_reg.c = _sra(cpu_reg.c); }  // CB 29
void sra_d() { cpu_reg.d = _sra(cpu_reg.d); }  // CB 2A
void sra_e() { cpu_reg.e = _sra(cpu_reg.e); }  // CB 2B
void sra_h() { cpu_reg.h = _sra(cpu_reg.h); }  // CB 2C
void sra_l() { cpu_reg.l = _sra(cpu_reg.l); }  // CB 2D
void sra_x() { mem_write(cpu_reg.hl, _sra(mem_read(cpu_reg.hl))); }  // CB 2E
void sra_a() { cpu_reg.a = _sra(cpu_reg.a); }  // CB 2F

u8 _swap(u8 r) {
    r = (r & 0x0F) << 4 | (r & 0xF0) >> 4;
    set_flags(!r, 0, 0, 0);
    return r;
}

void swap_b() { cpu_reg.b = _swap(cpu_reg.b); }  // CB 30
void swap_c() { cpu_reg.c = _swap(cpu_reg.c); }  // CB 31
void swap_d() { cpu_reg.d = _swap(cpu_reg.d); }  // CB 32
void swap_e() { cpu_reg.e = _swap(cpu_reg.e); }  // CB 33
void swap_h() { cpu_reg.h = _swap(cpu_reg.h); }  // CB 34
void swap_l() { cpu_reg.l = _swap(cpu_reg.l); }  // CB 35
void swap_x() { mem_write(cpu_reg.hl, _swap(mem_read(cpu_reg.hl))); }  // CB 36
void swap_a() { cpu_reg.a = _swap(cpu_reg.a); }  // CB 37

u8 _srl(u8 r) {
    u8 c = r & 1;
    r = (r >> 1) | ~(1 << 7);
    set_flags(!r, 0, 0, c);
    return r;
}

void srl_b() { cpu_reg.b = _srl(cpu_reg.b); }  // CB 38
void srl_c() { cpu_reg.c = _srl(cpu_reg.c); }  // CB 39
void srl_d() { cpu_reg.d = _srl(cpu_reg.d); }  // CB 3A
void srl_e() { cpu_reg.e = _srl(cpu_reg.e); }  // CB 3B
void srl_h() { cpu_reg.h = _srl(cpu_reg.h); }  // CB 3C
void srl_l() { cpu_reg.l = _srl(cpu_reg.l); }  // CB 3D
void srl_x() { mem_write(cpu_reg.hl, _srl(mem_read(cpu_reg.hl))); }  // CB 3E
void srl_a() { cpu_reg.a = _srl(cpu_reg.a); }  // CB 3F

void _bit(u8 r, u8 n) {
    bool z = !bit_read(r, n);
    set_flags(z, 0, 1, -1);
}

void bit0_b() { _bit(cpu_reg.b, 0); }  // CB 40
void bit0_c() { _bit(cpu_reg.c, 0); }  // CB 41
void bit0_d() { _bit(cpu_reg.d, 0); }  // CB 42
void bit0_e() { _bit(cpu_reg.e, 0); }  // CB 43
void bit0_h() { _bit(cpu_reg.h, 0); }  // CB 44
void bit0_l() { _bit(cpu_reg.l, 0); }  // CB 45
void bit0_x() { _bit(mem_read(cpu_reg.hl), 0); }  // CB 46
void bit0_a() { _bit(cpu_reg.a, 0); }  // CB 47

void bit1_b() { _bit(cpu_reg.b, 1); }  // CB 48
void bit1_c() { _bit(cpu_reg.c, 1); }  // CB 49
void bit1_d() { _bit(cpu_reg.d, 1); }  // CB 4A
void bit1_e() { _bit(cpu_reg.e, 1); }  // CB 4B
void bit1_h() { _bit(cpu_reg.h, 1); }  // CB 4C
void bit1_l() { _bit(cpu_reg.l, 1); }  // CB 4D
void bit1_x() { _bit(mem_read(cpu_reg.hl), 1); }  // CB 4E
void bit1_a() { _bit(cpu_reg.a, 1); }  // CB 4F

void bit2_b() { _bit(cpu_reg.b, 2); }  // CB 50
void bit2_c() { _bit(cpu_reg.c, 2); }  // CB 51
void bit2_d() { _bit(cpu_reg.d, 2); }  // CB 52
void bit2_e() { _bit(cpu_reg.e, 2); }  // CB 53
void bit2_h() { _bit(cpu_reg.h, 2); }  // CB 54
void bit2_l() { _bit(cpu_reg.l, 2); }  // CB 55
void bit2_x() { _bit(mem_read(cpu_reg.hl), 2); }  // CB 56
void bit2_a() { _bit(cpu_reg.a, 2); }  // CB 57

void bit3_b() { _bit(cpu_reg.b, 3); }  // CB 58
void bit3_c() { _bit(cpu_reg.c, 3); }  // CB 59
void bit3_d() { _bit(cpu_reg.d, 3); }  // CB 5A
void bit3_e() { _bit(cpu_reg.e, 3); }  // CB 5B
void bit3_h() { _bit(cpu_reg.h, 3); }  // CB 5C
void bit3_l() { _bit(cpu_reg.l, 3); }  // CB 5D
void bit3_x() { _bit(mem_read(cpu_reg.hl), 3); }  // CB 5E
void bit3_a() { _bit(cpu_reg.a, 3); }  // CB 5F

void bit4_b() { _bit(cpu_reg.b, 4); }  // CB 60
void bit4_c() { _bit(cpu_reg.c, 4); }  // CB 61
void bit4_d() { _bit(cpu_reg.d, 4); }  // CB 62
void bit4_e() { _bit(cpu_reg.e, 4); }  // CB 63
void bit4_h() { _bit(cpu_reg.h, 4); }  // CB 64
void bit4_l() { _bit(cpu_reg.l, 4); }  // CB 65
void bit4_x() { _bit(mem_read(cpu_reg.hl), 4); }  // CB 66
void bit4_a() { _bit(cpu_reg.a, 4); }  // CB 67

void bit5_b() { _bit(cpu_reg.b, 5); }  // CB 68
void bit5_c() { _bit(cpu_reg.c, 5); }  // CB 69
void bit5_d() { _bit(cpu_reg.d, 5); }  // CB 6A
void bit5_e() { _bit(cpu_reg.e, 5); }  // CB 6B
void bit5_h() { _bit(cpu_reg.h, 5); }  // CB 6C
void bit5_l() { _bit(cpu_reg.l, 5); }  // CB 6D
void bit5_x() { _bit(mem_read(cpu_reg.hl), 5); }  // CB 6E
void bit5_a() { _bit(cpu_reg.a, 5); }  // CB 6F

void bit6_b() { _bit(cpu_reg.b, 6); }  // CB 70
void bit6_c() { _bit(cpu_reg.c, 6); }  // CB 71
void bit6_d() { _bit(cpu_reg.d, 6); }  // CB 72
void bit6_e() { _bit(cpu_reg.e, 6); }  // CB 73
void bit6_h() { _bit(cpu_reg.h, 6); }  // CB 74
void bit6_l() { _bit(cpu_reg.l, 6); }  // CB 75
void bit6_x() { _bit(mem_read(cpu_reg.hl), 6); }  // CB 76
void bit6_a() { _bit(cpu_reg.a, 6); }  // CB 77

void bit7_b() { _bit(cpu_reg.b, 7); }  // CB 78
void bit7_c() { _bit(cpu_reg.c, 7); }  // CB 79
void bit7_d() { _bit(cpu_reg.d, 7); }  // CB 7A
void bit7_e() { _bit(cpu_reg.e, 7); }  // CB 7B
void bit7_h() { _bit(cpu_reg.h, 7); }  // CB 7C
void bit7_l() { _bit(cpu_reg.l, 7); }  // CB 7D
void bit7_x() { _bit(mem_read(cpu_reg.hl), 7); }  // CB 7E
void bit7_a() { _bit(cpu_reg.a, 7); }  // CB 7F

void res0_b() { cpu_reg.b = bit_clear(cpu_reg.b, 0); }  // CB 80
void res0_c() { cpu_reg.c = bit_clear(cpu_reg.c, 0); }  // CB 81
void res0_d() { cpu_reg.d = bit_clear(cpu_reg.d, 0); }  // CB 82
void res0_e() { cpu_reg.e = bit_clear(cpu_reg.e, 0); }  // CB 83
void res0_h() { cpu_reg.h = bit_clear(cpu_reg.h, 0); }  // CB 84
void res0_l() { cpu_reg.l = bit_clear(cpu_reg.l, 0); }  // CB 85
void res0_x() { mem_write(cpu_reg.hl, bit_clear(mem_read(cpu_reg.hl), 0)); }  // CB 86
void res0_a() { cpu_reg.a = bit_clear(cpu_reg.a, 0); }  // CB 87

void res1_b() { cpu_reg.b = bit_clear(cpu_reg.b, 1); }  // CB 88
void res1_c() { cpu_reg.c = bit_clear(cpu_reg.c, 1); }  // CB 89
void res1_d() { cpu_reg.d = bit_clear(cpu_reg.d, 1); }  // CB 8A
void res1_e() { cpu_reg.e = bit_clear(cpu_reg.e, 1); }  // CB 8B
void res1_h() { cpu_reg.h = bit_clear(cpu_reg.h, 1); }  // CB 8C
void res1_l() { cpu_reg.l = bit_clear(cpu_reg.l, 1); }  // CB 8D
void res1_x() { mem_write(cpu_reg.hl, bit_clear(mem_read(cpu_reg.hl), 1)); }  // CB 8E
void res1_a() { cpu_reg.a = bit_clear(cpu_reg.a, 1); }  // CB 8F

void res2_b() { cpu_reg.b = bit_clear(cpu_reg.b, 2); }  // CB 90
void res2_c() { cpu_reg.c = bit_clear(cpu_reg.c, 2); }  // CB 91
void res2_d() { cpu_reg.d = bit_clear(cpu_reg.d, 2); }  // CB 92
void res2_e() { cpu_reg.e = bit_clear(cpu_reg.e, 2); }  // CB 93
void res2_h() { cpu_reg.h = bit_clear(cpu_reg.h, 2); }  // CB 94
void res2_l() { cpu_reg.l = bit_clear(cpu_reg.l, 2); }  // CB 95
void res2_x() { mem_write(cpu_reg.hl, bit_clear(mem_read(cpu_reg.hl), 2)); }  // CB 96
void res2_a() { cpu_reg.a = bit_clear(cpu_reg.a, 2); }  // CB 97

void res3_b() { cpu_reg.b = bit_clear(cpu_reg.b, 3); }  // CB 98
void res3_c() { cpu_reg.c = bit_clear(cpu_reg.c, 3); }  // CB 99
void res3_d() { cpu_reg.d = bit_clear(cpu_reg.d, 3); }  // CB 9A
void res3_e() { cpu_reg.e = bit_clear(cpu_reg.e, 3); }  // CB 9B
void res3_h() { cpu_reg.h = bit_clear(cpu_reg.h, 3); }  // CB 9C
void res3_l() { cpu_reg.l = bit_clear(cpu_reg.l, 3); }  // CB 9D
void res3_x() { mem_write(cpu_reg.hl, bit_clear(mem_read(cpu_reg.hl), 3)); }  // CB 9E
void res3_a() { cpu_reg.a = bit_clear(cpu_reg.a, 3); }  // CB 9F

void res4_b() { cpu_reg.b = bit_clear(cpu_reg.b, 4); }  // CB A0
void res4_c() { cpu_reg.c = bit_clear(cpu_reg.c, 4); }  // CB A1
void res4_d() { cpu_reg.d = bit_clear(cpu_reg.d, 4); }  // CB A2
void res4_e() { cpu_reg.e = bit_clear(cpu_reg.e, 4); }  // CB A3
void res4_h() { cpu_reg.h = bit_clear(cpu_reg.h, 4); }  // CB A4
void res4_l() { cpu_reg.l = bit_clear(cpu_reg.l, 4); }  // CB A5
void res4_x() { mem_write(cpu_reg.hl, bit_clear(mem_read(cpu_reg.hl), 4)); }  // CB A6
void res4_a() { cpu_reg.a = bit_clear(cpu_reg.a, 4); }  // CB A7

void res5_b() { cpu_reg.b = bit_clear(cpu_reg.b, 5); }  // CB A8
void res5_c() { cpu_reg.c = bit_clear(cpu_reg.c, 5); }  // CB A9
void res5_d() { cpu_reg.d = bit_clear(cpu_reg.d, 5); }  // CB AA
void res5_e() { cpu_reg.e = bit_clear(cpu_reg.e, 5); }  // CB AB
void res5_h() { cpu_reg.h = bit_clear(cpu_reg.h, 5); }  // CB AC
void res5_l() { cpu_reg.l = bit_clear(cpu_reg.l, 5); }  // CB AD
void res5_x() { mem_write(cpu_reg.hl, bit_clear(mem_read(cpu_reg.hl), 5)); }  // CB AE
void res5_a() { cpu_reg.a = bit_clear(cpu_reg.a, 5); }  // CB AF

void res6_b() { cpu_reg.b = bit_clear(cpu_reg.b, 6); }  // CB B0
void res6_c() { cpu_reg.c = bit_clear(cpu_reg.c, 6); }  // CB B1
void res6_d() { cpu_reg.d = bit_clear(cpu_reg.d, 6); }  // CB B2
void res6_e() { cpu_reg.e = bit_clear(cpu_reg.e, 6); }  // CB B3
void res6_h() { cpu_reg.h = bit_clear(cpu_reg.h, 6); }  // CB B4
void res6_l() { cpu_reg.l = bit_clear(cpu_reg.l, 6); }  // CB B5
void res6_x() { mem_write(cpu_reg.hl, bit_clear(mem_read(cpu_reg.hl), 6)); }  // CB B6
void res6_a() { cpu_reg.a = bit_clear(cpu_reg.a, 6); }  // CB B7

void res7_b() { cpu_reg.b = bit_clear(cpu_reg.b, 7); }  // CB B8
void res7_c() { cpu_reg.c = bit_clear(cpu_reg.c, 7); }  // CB B9
void res7_d() { cpu_reg.d = bit_clear(cpu_reg.d, 7); }  // CB BA
void res7_e() { cpu_reg.e = bit_clear(cpu_reg.e, 7); }  // CB BB
void res7_h() { cpu_reg.h = bit_clear(cpu_reg.h, 7); }  // CB BC
void res7_l() { cpu_reg.l = bit_clear(cpu_reg.l, 7); }  // CB BD
void res7_x() { mem_write(cpu_reg.hl, bit_clear(mem_read(cpu_reg.hl), 7)); }  // CB BE
void res7_a() { cpu_reg.a = bit_clear(cpu_reg.a, 7); }  // CB BF

void set0_b() { cpu_reg.b = bit_set(cpu_reg.b, 0); }  // CB C0
void set0_c() { cpu_reg.c = bit_set(cpu_reg.c, 0); }  // CB C1
void set0_d() { cpu_reg.d = bit_set(cpu_reg.d, 0); }  // CB C2
void set0_e() { cpu_reg.e = bit_set(cpu_reg.e, 0); }  // CB C3
void set0_h() { cpu_reg.h = bit_set(cpu_reg.h, 0); }  // CB C4
void set0_l() { cpu_reg.l = bit_set(cpu_reg.l, 0); }  // CB C5
void set0_x() { mem_write(cpu_reg.hl, bit_set(mem_read(cpu_reg.hl), 0)); }  // CB C6
void set0_a() { cpu_reg.a = bit_set(cpu_reg.a, 0); }  // CB C7

void set1_b() { cpu_reg.b = bit_set(cpu_reg.b, 1); }  // CB C8
void set1_c() { cpu_reg.c = bit_set(cpu_reg.c, 1); }  // CB C9
void set1_d() { cpu_reg.d = bit_set(cpu_reg.d, 1); }  // CB CA
void set1_e() { cpu_reg.e = bit_set(cpu_reg.e, 1); }  // CB CB
void set1_h() { cpu_reg.h = bit_set(cpu_reg.h, 1); }  // CB CC
void set1_l() { cpu_reg.l = bit_set(cpu_reg.l, 1); }  // CB CD
void set1_x() { mem_write(cpu_reg.hl, bit_set(mem_read(cpu_reg.hl), 1)); }  // CB CE
void set1_a() { cpu_reg.a = bit_set(cpu_reg.a, 1); }  // CB CF

void set2_b() { cpu_reg.b = bit_set(cpu_reg.b, 2); }  // CB D0
void set2_c() { cpu_reg.c = bit_set(cpu_reg.c, 2); }  // CB D1
void set2_d() { cpu_reg.d = bit_set(cpu_reg.d, 2); }  // CB D2
void set2_e() { cpu_reg.e = bit_set(cpu_reg.e, 2); }  // CB D3
void set2_h() { cpu_reg.h = bit_set(cpu_reg.h, 2); }  // CB D4
void set2_l() { cpu_reg.l = bit_set(cpu_reg.l, 2); }  // CB D5
void set2_x() { mem_write(cpu_reg.hl, bit_set(mem_read(cpu_reg.hl), 2)); }  // CB D6
void set2_a() { cpu_reg.a = bit_set(cpu_reg.a, 2); }  // CB D7

void set3_b() { cpu_reg.b = bit_set(cpu_reg.b, 3); }  // CB D8
void set3_c() { cpu_reg.c = bit_set(cpu_reg.c, 3); }  // CB D9
void set3_d() { cpu_reg.d = bit_set(cpu_reg.d, 3); }  // CB DA
void set3_e() { cpu_reg.e = bit_set(cpu_reg.e, 3); }  // CB DB
void set3_h() { cpu_reg.h = bit_set(cpu_reg.h, 3); }  // CB DC
void set3_l() { cpu_reg.l = bit_set(cpu_reg.l, 3); }  // CB DD
void set3_x() { mem_write(cpu_reg.hl, bit_set(mem_read(cpu_reg.hl), 3)); }  // CB DE
void set3_a() { cpu_reg.a = bit_set(cpu_reg.a, 3); }  // CB DF

void set4_b() { cpu_reg.b = bit_set(cpu_reg.b, 4); }  // CB E0
void set4_c() { cpu_reg.c = bit_set(cpu_reg.c, 4); }  // CB E1
void set4_d() { cpu_reg.d = bit_set(cpu_reg.d, 4); }  // CB E2
void set4_e() { cpu_reg.e = bit_set(cpu_reg.e, 4); }  // CB E3
void set4_h() { cpu_reg.h = bit_set(cpu_reg.h, 4); }  // CB E4
void set4_l() { cpu_reg.l = bit_set(cpu_reg.l, 4); }  // CB E5
void set4_x() { mem_write(cpu_reg.hl, bit_set(mem_read(cpu_reg.hl), 4)); }  // CB E6
void set4_a() { cpu_reg.a = bit_set(cpu_reg.a, 4); }  // CB E7

void set5_b() { cpu_reg.b = bit_set(cpu_reg.b, 5); }  // CB E8
void set5_c() { cpu_reg.c = bit_set(cpu_reg.c, 5); }  // CB E9
void set5_d() { cpu_reg.d = bit_set(cpu_reg.d, 5); }  // CB EA
void set5_e() { cpu_reg.e = bit_set(cpu_reg.e, 5); }  // CB EB
void set5_h() { cpu_reg.h = bit_set(cpu_reg.h, 5); }  // CB EC
void set5_l() { cpu_reg.l = bit_set(cpu_reg.l, 5); }  // CB ED
void set5_x() { mem_write(cpu_reg.hl, bit_set(mem_read(cpu_reg.hl), 5)); }  // CB EE
void set5_a() { cpu_reg.a = bit_set(cpu_reg.a, 5); }  // CB EF

void set6_b() { cpu_reg.b = bit_set(cpu_reg.b, 6); }  // CB F0
void set6_c() { cpu_reg.c = bit_set(cpu_reg.c, 6); }  // CB F1
void set6_d() { cpu_reg.d = bit_set(cpu_reg.d, 6); }  // CB F2
void set6_e() { cpu_reg.e = bit_set(cpu_reg.e, 6); }  // CB F3
void set6_h() { cpu_reg.h = bit_set(cpu_reg.h, 6); }  // CB F4
void set6_l() { cpu_reg.l = bit_set(cpu_reg.l, 6); }  // CB F5
void set6_x() { mem_write(cpu_reg.hl, bit_set(mem_read(cpu_reg.hl), 6)); }  // CB F6
void set6_a() { cpu_reg.a = bit_set(cpu_reg.a, 6); }  // CB F7

void set7_b() { cpu_reg.b = bit_set(cpu_reg.b, 7); }  // CB F8
void set7_c() { cpu_reg.c = bit_set(cpu_reg.c, 7); }  // CB F9
void set7_d() { cpu_reg.d = bit_set(cpu_reg.d, 7); }  // CB FA
void set7_e() { cpu_reg.e = bit_set(cpu_reg.e, 7); }  // CB FB
void set7_h() { cpu_reg.h = bit_set(cpu_reg.h, 7); }  // CB FC
void set7_l() { cpu_reg.l = bit_set(cpu_reg.l, 7); }  // CB FD
void set7_x() { mem_write(cpu_reg.hl, bit_set(mem_read(cpu_reg.hl), 7)); }  // CB FE
void set7_a() { cpu_reg.a = bit_set(cpu_reg.a, 7); }  // CB FF
