#include "cpu.h"
#include "mem.h"
#include "stack.h"
#include "instruction.h"

// read an immediate 8-bit value
u8 read_n8() {
    return mem_read(cpu_reg.pc++);
}

// read an immediate 16-bit value
u16 read_n16() {
    u16 nn = mem_read16(cpu_reg.pc);
    cpu_reg.pc += 2;
    return nn;
}

void nop() {}
void halt() {}

void ld_ab() { cpu_reg.a = cpu_reg.b; }
void ld_ac() { cpu_reg.a = cpu_reg.c; }
void ld_ad() { cpu_reg.a = cpu_reg.d; }
void ld_ae() { cpu_reg.a = cpu_reg.e; }
void ld_ah() { cpu_reg.a = cpu_reg.h; }
void ld_al() { cpu_reg.a = cpu_reg.l; }
void ld_aa() { cpu_reg.a = cpu_reg.a; }
void ld_ax() { cpu_reg.a = mem_read(cpu_reg.hl); }
void ld_an() { cpu_reg.a = read_n8(); }

void ld_bb() { cpu_reg.b = cpu_reg.b; }
void ld_bc() { cpu_reg.b = cpu_reg.c; }
void ld_bd() { cpu_reg.b = cpu_reg.d; }
void ld_be() { cpu_reg.b = cpu_reg.e; }
void ld_bh() { cpu_reg.b = cpu_reg.h; }
void ld_bl() { cpu_reg.b = cpu_reg.l; }
void ld_ba() { cpu_reg.b = cpu_reg.a; }
void ld_bx() { cpu_reg.b = mem_read(cpu_reg.hl); }
void ld_bn() { cpu_reg.b = read_n8(); }

void ld_cb() { cpu_reg.c = cpu_reg.b; }
void ld_cc() { cpu_reg.c = cpu_reg.c; }
void ld_cd() { cpu_reg.c = cpu_reg.d; }
void ld_ce() { cpu_reg.c = cpu_reg.e; }
void ld_ch() { cpu_reg.c = cpu_reg.h; }
void ld_cl() { cpu_reg.c = cpu_reg.l; }
void ld_ca() { cpu_reg.c = cpu_reg.a; }
void ld_cx() { cpu_reg.c = mem_read(cpu_reg.hl); }
void ld_cn() { cpu_reg.c = read_n8(); }

void ld_db() { cpu_reg.d = cpu_reg.b; }
void ld_dc() { cpu_reg.d = cpu_reg.c; }
void ld_dd() { cpu_reg.d = cpu_reg.d; }
void ld_de() { cpu_reg.d = cpu_reg.e; }
void ld_dh() { cpu_reg.d = cpu_reg.h; }
void ld_dl() { cpu_reg.d = cpu_reg.l; }
void ld_da() { cpu_reg.d = cpu_reg.a; }
void ld_dx() { cpu_reg.d = mem_read(cpu_reg.hl); }
void ld_dn() { cpu_reg.d = read_n8(); }

void ld_eb() { cpu_reg.e = cpu_reg.b; }
void ld_ec() { cpu_reg.e = cpu_reg.c; }
void ld_ed() { cpu_reg.e = cpu_reg.d; }
void ld_ee() { cpu_reg.e = cpu_reg.e; }
void ld_eh() { cpu_reg.e = cpu_reg.h; }
void ld_el() { cpu_reg.e = cpu_reg.l; }
void ld_ea() { cpu_reg.e = cpu_reg.a; }
void ld_ex() { cpu_reg.e = mem_read(cpu_reg.hl); }
void ld_en() { cpu_reg.e = read_n8(); }

void ld_hb() { cpu_reg.h = cpu_reg.b; }
void ld_hc() { cpu_reg.h = cpu_reg.c; }
void ld_hd() { cpu_reg.h = cpu_reg.d; }
void ld_he() { cpu_reg.h = cpu_reg.e; }
void ld_hh() { cpu_reg.h = cpu_reg.h; }
void ld_hl() { cpu_reg.h = cpu_reg.l; }
void ld_ha() { cpu_reg.h = cpu_reg.a; }
void ld_hx() { cpu_reg.h = mem_read(cpu_reg.hl); }
void ld_hn() { cpu_reg.h = read_n8(); }

void ld_lb() { cpu_reg.l = cpu_reg.b; }
void ld_lc() { cpu_reg.l = cpu_reg.c; }
void ld_ld() { cpu_reg.l = cpu_reg.d; }
void ld_le() { cpu_reg.l = cpu_reg.e; }
void ld_lh() { cpu_reg.l = cpu_reg.h; }
void ld_ll() { cpu_reg.l = cpu_reg.l; }
void ld_la() { cpu_reg.l = cpu_reg.a; }
void ld_lx() { cpu_reg.l = mem_read(cpu_reg.hl); }
void ld_ln() { cpu_reg.l = read_n8(); }

void ld_xb() { mem_write(cpu_reg.hl, cpu_reg.b); }
void ld_xc() { mem_write(cpu_reg.hl, cpu_reg.c); }
void ld_xd() { mem_write(cpu_reg.hl, cpu_reg.d); }
void ld_xe() { mem_write(cpu_reg.hl, cpu_reg.e); }
void ld_xh() { mem_write(cpu_reg.hl, cpu_reg.h); }
void ld_xl() { mem_write(cpu_reg.hl, cpu_reg.l); }
void ld_xa() { mem_write(cpu_reg.hl, cpu_reg.a); }
void ld_xn() { mem_write(cpu_reg.hl, read_n8()); }

void ld_a_bc() { cpu_reg.a = mem_read(cpu_reg.bc); }
void ld_a_de() { cpu_reg.a = mem_read(cpu_reg.de); }

void ld_bc_a() { mem_write(cpu_reg.bc, cpu_reg.a); }
void ld_de_a() { mem_write(cpu_reg.de, cpu_reg.a); }

void ld_nn_a() { mem_write(read_n16(), cpu_reg.a); }
void ld_a_nn() { cpu_reg.a = mem_read(read_n16()); }

void ld_bc_nn() { cpu_reg.bc = read_n16(); }
void ld_de_nn() { cpu_reg.de = read_n16(); }
void ld_hl_nn() { cpu_reg.hl = read_n16(); }
void ld_sp_nn() { cpu_reg.sp = read_n16(); }

void ldio_c_a() { cpu_reg.a = mem_read(0xFF00 + cpu_reg.c); }
void ldio_a_c() { mem_write(0xFF00 + cpu_reg.c, cpu_reg.a); }
void ldio_n_a() { cpu_reg.a = mem_read(0xFF00 + read_n8()); }
void ldio_a_n() { mem_write(0xFF00 + read_n8(), cpu_reg.a); }

void ldd_ax() { cpu_reg.a = mem_read(cpu_reg.hl--); }
void ldd_xa() { mem_write(cpu_reg.hl--, cpu_reg.a); }
void ldi_ax() { cpu_reg.a = mem_read(cpu_reg.hl++); }
void ldi_xa() { mem_write(cpu_reg.hl++, cpu_reg.a); }

void ld_sp_hl() { cpu_reg.sp = cpu_reg.hl; }

void ld_hl_sp_n() {
    u8 x = read_n8();
    cpu_reg.hl = cpu_reg.sp + x;
    bool z = false;
    bool n = false;
    bool h = (cpu_reg.sp & 0xF) + (x & 0xF) > 0xF;
    bool c = (cpu_reg.sp & 0xFF) + (x & 0xFF) > 0xFF;
    set_flags(z, n, h, c);
}

void ld_nn_sp() { mem_write16(read_n16(), cpu_reg.sp); }

void push_af() { stack_push16(cpu_reg.af); }
void push_bc() { stack_push16(cpu_reg.bc); }
void push_de() { stack_push16(cpu_reg.de); }
void push_hl() { stack_push16(cpu_reg.hl); }

void pop_af() { cpu_reg.af = stack_pop16() & 0x0FFF; }
void pop_bc() { cpu_reg.bc = stack_pop16(); }
void pop_de() { cpu_reg.de = stack_pop16(); }
void pop_hl() { cpu_reg.hl = stack_pop16(); }

u8 _add(u8 a, u8 b) {
    u16 result = a + b;
    bool z = !result;
    bool n = false;
    bool h = (a & 0xF) + (b & 0xF) > 0xF;
    bool c = result > 0xFF;
    set_flags(z, n, h, c);
    return result;
}

void add_ab() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.b); }
void add_ac() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.c); }
void add_ad() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.d); }
void add_ae() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.e); }
void add_ah() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.h); }
void add_al() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.l); }
void add_aa() { cpu_reg.a = _add(cpu_reg.a, cpu_reg.a); }
void add_ax() { cpu_reg.a = _add(cpu_reg.a, mem_read(cpu_reg.hl)); }
void add_an() { cpu_reg.a = _add(cpu_reg.a, read_n8()); }

u8 _adc(u8 a, u8 b) {
    u16 result = a + b + flag_c();
    bool z = !result;
    bool n = false;
    bool h = (a & 0xF) + (b & 0xF) + flag_c() > 0xF;
    bool c = result > 0xFF;
    set_flags(z, n, h, c);
    return result;
}

void adc_ab() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.b); }
void adc_ac() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.c); }
void adc_ad() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.d); }
void adc_ae() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.e); }
void adc_ah() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.h); }
void adc_al() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.l); }
void adc_aa() { cpu_reg.a = _adc(cpu_reg.a, cpu_reg.a); }
void adc_ax() { cpu_reg.a = _adc(cpu_reg.a, mem_read(cpu_reg.hl)); }
void adc_an() { cpu_reg.a = _adc(cpu_reg.a, read_n8()); }

u8 _sub(i8 a, i8 b) {
    i8 result = a - b;
    bool z = !result;
    bool n = true;
    bool h = (a & 0xF) - (b & 0xF) < 0;
    bool c = result < 0;
    set_flags(z, n, h, c);
    return result;
}

void sub_ab() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.b); }
void sub_ac() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.c); }
void sub_ad() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.d); }
void sub_ae() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.e); }
void sub_ah() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.h); }
void sub_al() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.l); }
void sub_aa() { cpu_reg.a = _sub(cpu_reg.a, cpu_reg.a); }
void sub_ax() { cpu_reg.a = _sub(cpu_reg.a, mem_read(cpu_reg.hl)); }
void sub_an() { cpu_reg.a = _sub(cpu_reg.a, read_n8()); }

u8 _sbc(i8 a, i8 b) {
    i8 result = a - b - flag_c();
    bool z = !result;
    bool n = true;
    bool h = (a & 0xF) - (b & 0xF) - flag_c() < 0;
    bool c = result < 0;
    set_flags(z, n, h, c);
    return result;
}

void sbc_ab() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.b); }
void sbc_ac() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.c); }
void sbc_ad() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.d); }
void sbc_ae() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.e); }
void sbc_ah() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.h); }
void sbc_al() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.l); }
void sbc_aa() { cpu_reg.a = _sbc(cpu_reg.a, cpu_reg.a); }
void sbc_ax() { cpu_reg.a = _sbc(cpu_reg.a, mem_read(cpu_reg.hl)); }
void sbc_an() { cpu_reg.a = _sbc(cpu_reg.a, read_n8()); }

u8 _and(u8 a, u8 b) {
    u8 result = a & b;
    bool z = !result;
    bool n = false;
    bool h = true;
    bool c = false;
    set_flags(z, n, h, c);
    return result;
}

void and_ab() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.b); }
void and_ac() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.c); }
void and_ad() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.d); }
void and_ae() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.e); }
void and_ah() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.h); }
void and_al() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.l); }
void and_aa() { cpu_reg.a = _and(cpu_reg.a, cpu_reg.a); }
void and_ax() { cpu_reg.a = _and(cpu_reg.a, mem_read(cpu_reg.hl)); }
void and_an() { cpu_reg.a = _and(cpu_reg.a, read_n8()); }

u8 _xor(u8 a, u8 b) {
    u8 result = a ^ b;
    bool z = !result;
    bool n = false;
    bool h = false;
    bool c = false;
    set_flags(z, n, h, c);
    return result;
}

void xor_ab() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.b); }
void xor_ac() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.c); }
void xor_ad() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.d); }
void xor_ae() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.e); }
void xor_ah() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.h); }
void xor_al() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.l); }
void xor_aa() { cpu_reg.a = _xor(cpu_reg.a, cpu_reg.a); }
void xor_ax() { cpu_reg.a = _xor(cpu_reg.a, mem_read(cpu_reg.hl)); }
void xor_an() { cpu_reg.a = _xor(cpu_reg.a, read_n8()); }

u8 _or(u8 a, u8 b) {
    u8 result = a | b;
    bool z = !result;
    bool n = false;
    bool h = false;
    bool c = false;
    set_flags(z, n, h, c);
    return result;
}

void or_ab() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.b); }
void or_ac() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.c); }
void or_ad() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.d); }
void or_ae() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.e); }
void or_ah() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.h); }
void or_al() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.l); }
void or_aa() { cpu_reg.a = _or(cpu_reg.a, cpu_reg.a); }
void or_ax() { cpu_reg.a = _or(cpu_reg.a, mem_read(cpu_reg.hl)); }
void or_an() { cpu_reg.a = _or(cpu_reg.a, read_n8()); }

u8 _cmp(i8 a, i8 b) {
    i8 result = a - b;
    bool z = !result;
    bool n = true;
    bool h = (a & 0xF) - (b & 0xF) < 0;
    bool c = result < 0;
    set_flags(z, n, h, c);
    return result;
}

void cmp_ab() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.b); }
void cmp_ac() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.c); }
void cmp_ad() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.d); }
void cmp_ae() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.e); }
void cmp_ah() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.h); }
void cmp_al() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.l); }
void cmp_aa() { cpu_reg.a = _cmp(cpu_reg.a, cpu_reg.a); }
void cmp_ax() { cpu_reg.a = _cmp(cpu_reg.a, mem_read(cpu_reg.hl)); }
void cmp_an() { cpu_reg.a = _cmp(cpu_reg.a, read_n8()); }

u8 _inc(u8 x) {
    x++;
    bool z = !x;
    bool n = false;
    bool h = (x & 0xF) == 0;
    set_flags(z, n, h, -1);
    return x;
}

void inc_b() { cpu_reg.b = _inc(cpu_reg.b); }
void inc_c() { cpu_reg.c = _inc(cpu_reg.c); }
void inc_d() { cpu_reg.d = _inc(cpu_reg.d); }
void inc_e() { cpu_reg.e = _inc(cpu_reg.e); }
void inc_h() { cpu_reg.h = _inc(cpu_reg.h); }
void inc_l() { cpu_reg.l = _inc(cpu_reg.l); }
void inc_a() { cpu_reg.a = _inc(cpu_reg.a); }
void inc_x() { mem_write(cpu_reg.hl, _inc(mem_read(cpu_reg.hl))); }

u8 _dec(u8 x) {
    x--;
    bool z = !x;
    bool n = true;
    bool h = (x & 0xF) == 0xF;
    set_flags(z, n, h, -1);
    return x;
}

void dec_b() { cpu_reg.b = _dec(cpu_reg.b); }
void dec_c() { cpu_reg.c = _dec(cpu_reg.c); }
void dec_d() { cpu_reg.d = _dec(cpu_reg.d); }
void dec_e() { cpu_reg.e = _dec(cpu_reg.e); }
void dec_h() { cpu_reg.h = _dec(cpu_reg.h); }
void dec_l() { cpu_reg.l = _dec(cpu_reg.l); }
void dec_a() { cpu_reg.a = _dec(cpu_reg.a); }
void dec_x() { mem_write(cpu_reg.hl, _dec(mem_read(cpu_reg.hl))); }

u16 _add16(u16 a, u16 b) {
    u16 result = a + b;
    bool n = false;
    bool h = (a & 0xFFF) + (b & 0xFFF) > 0xFFF;
    bool c = (result < a) && (result < b);
    set_flags(-1, n, h, c);
    return result;
}

void add16_bc() { cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.bc); }
void add16_de() { cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.de); }
void add16_hl() { cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.hl); }
void add16_sp() { cpu_reg.hl = _add16(cpu_reg.hl, cpu_reg.sp); }

void add_sp_n() {
    i8 x = read_n8();
    bool z = false;
    bool n = false;
    bool h = (cpu_reg.sp & 0xF) + (x & 0xF) > 0xF;
    bool c = (cpu_reg.sp & 0xFF) + (x & 0xFF) > 0xFF;
    set_flags(z, n, h, c);
    cpu_reg.sp += x;
}

void inc16_bc() { cpu_reg.bc++; }
void inc16_de() { cpu_reg.de++; }
void inc16_hl() { cpu_reg.hl++; }
void inc16_sp() { cpu_reg.sp++; }

void dec16_bc() { cpu_reg.bc--; }
void dec16_de() { cpu_reg.de--; }
void dec16_hl() { cpu_reg.hl--; }
void dec16_sp() { cpu_reg.sp--; }
