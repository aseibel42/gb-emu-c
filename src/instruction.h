#pragma once

#include "util.h"

typedef void (*Instruction)(void);

extern Instruction op[0x100];

void nop();
void halt( /* TODO */);

void ld_ab();
void ld_ac();
void ld_ad();
void ld_ae();
void ld_ah();
void ld_al();
void ld_aa();
void ld_ax();
void ld_an();

void ld_bb();
void ld_bc();
void ld_bd();
void ld_be();
void ld_bh();
void ld_bl();
void ld_ba();
void ld_bx();
void ld_bn();

void ld_cb();
void ld_cc();
void ld_cd();
void ld_ce();
void ld_ch();
void ld_cl();
void ld_ca();
void ld_cx();
void ld_cn();

void ld_db();
void ld_dc();
void ld_dd();
void ld_de();
void ld_dh();
void ld_dl();
void ld_da();
void ld_dx();
void ld_dn();

void ld_eb();
void ld_ec();
void ld_ed();
void ld_ee();
void ld_eh();
void ld_el();
void ld_ea();
void ld_ex();
void ld_en();

void ld_hb();
void ld_hc();
void ld_hd();
void ld_he();
void ld_hh();
void ld_hl();
void ld_ha();
void ld_hx();
void ld_hn();

void ld_lb();
void ld_lc();
void ld_ld();
void ld_le();
void ld_lh();
void ld_ll();
void ld_la();
void ld_lx();
void ld_ln();

void ld_xb();
void ld_xc();
void ld_xd();
void ld_xe();
void ld_xh();
void ld_xl();
void ld_xa();
void ld_xn();

void ld_a_bc();
void ld_a_de();

void ld_bc_a();
void ld_de_a();

void ld_a_nn();
void ld_nn_a();

void ld_bc_nn();
void ld_de_nn();
void ld_hl_nn();
void ld_sp_nn();

void ldio_a_c();
void ldio_c_a();
void ldio_a_n();
void ldio_n_a();

void ldd_ax();
void ldd_xa();
void ldi_ax();
void ldi_xa();

void ld_sp_hl();

void ld_hl_sp_n();

void ld_nn_sp();

void push_af();
void push_bc();
void push_de();
void push_hl();

void pop_af();
void pop_bc();
void pop_de();
void pop_hl();

void add_ab();
void add_ac();
void add_ad();
void add_ae();
void add_ah();
void add_al();
void add_aa();
void add_ax();
void add_an();

void adc_ab();
void adc_ac();
void adc_ad();
void adc_ae();
void adc_ah();
void adc_al();
void adc_aa();
void adc_ax();
void adc_an();

void sub_ab();
void sub_ac();
void sub_ad();
void sub_ae();
void sub_ah();
void sub_al();
void sub_aa();
void sub_ax();
void sub_an();

void sbc_ab();
void sbc_ac();
void sbc_ad();
void sbc_ae();
void sbc_ah();
void sbc_al();
void sbc_aa();
void sbc_ax();
void sbc_an();

void and_ab();
void and_ac();
void and_ad();
void and_ae();
void and_ah();
void and_al();
void and_aa();
void and_ax();
void and_an();

void xor_ab();
void xor_ac();
void xor_ad();
void xor_ae();
void xor_ah();
void xor_al();
void xor_aa();
void xor_ax();
void xor_an();

void or_ab();
void or_ac();
void or_ad();
void or_ae();
void or_ah();
void or_al();
void or_aa();
void or_ax();
void or_an();

void cmp_ab();
void cmp_ac();
void cmp_ad();
void cmp_ae();
void cmp_ah();
void cmp_al();
void cmp_aa();
void cmp_ax();
void cmp_an();

void inc_b();
void inc_c();
void inc_d();
void inc_e();
void inc_h();
void inc_l();
void inc_a();
void inc_x();

void dec_b();
void dec_c();
void dec_d();
void dec_e();
void dec_h();
void dec_l();
void dec_a();
void dec_x();

void add16_bc();
void add16_de();
void add16_hl();
void add16_sp();
void add_sp_n();

void inc16_bc();
void inc16_de();
void inc16_hl();
void inc16_sp();

void dec16_bc();
void dec16_de();
void dec16_hl();
void dec16_sp();
