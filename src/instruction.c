#include "cpu.h"
#include "mem.h"
#include "stack.h"
#include "instruction.h"

/*
 * Machine control instructions
*/

void nop() {}  // 0x00
void halt() {}  // 0x76

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

void ld_sp_hl() { cpu_reg.sp = cpu_reg.hl; }  // 0xF9

void ld_hl_sp_n() {
    u8 x = fetch();
    cpu_reg.hl = cpu_reg.sp + x;
    bool z = false;
    bool n = false;
    bool h = (cpu_reg.sp & 0xF) + (x & 0xF) > 0xF;
    bool c = (cpu_reg.sp & 0xFF) + (x & 0xFF) > 0xFF;
    set_flags(z, n, h, c);
}  // 0xF8

void ld_nn_sp() { mem_write16(fetch16(), cpu_reg.sp); }  // 0x08

void push_af() { stack_push16(cpu_reg.af); }  // 0xF5
void push_bc() { stack_push16(cpu_reg.bc); }  // 0xC5
void push_de() { stack_push16(cpu_reg.de); }  // 0xD5
void push_hl() { stack_push16(cpu_reg.hl); }  // 0xE5

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

void add16_bc() { cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.bc); }  // 0x09
void add16_de() { cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.de); }  // 0x19
void add16_hl() { cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.hl); }  // 0x29
void add16_sp() { cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.sp); }  // 0x39

void add_sp_n() {
    i8 x = fetch();
    bool z = false;
    bool n = false;
    bool h = (cpu_reg.sp & 0xF) + (x & 0xF) > 0xF;
    bool c = (cpu_reg.sp & 0xFF) + (x & 0xFF) > 0xFF;
    set_flags(z, n, h, c);
    cpu_reg.sp += x;
}  // 0xE8

void inc16_bc() { cpu_reg.bc++; }  // 0x03
void inc16_de() { cpu_reg.de++; }  // 0x13
void inc16_hl() { cpu_reg.hl++; }  // 0x23
void inc16_sp() { cpu_reg.sp++; }  // 0x33

void dec16_bc() { cpu_reg.bc--; }  // 0x0B
void dec16_de() { cpu_reg.de--; }  // 0x1B
void dec16_hl() { cpu_reg.hl--; }  // 0x2B
void dec16_sp() { cpu_reg.sp--; }  // 0x3B
