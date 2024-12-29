#pragma once

#include "util.h"

typedef void (*Instruction)(void);

extern Instruction op[0x100];
extern Instruction op_cb[0x100];

void nop();
void halt();
void stop();

void ei();
void di();
void ret_i();

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

void rotl_a();
void rotr_a();
void rotlc_a();
void rotrc_a();

void cmpl_a();
void dec_adj_a();

void ccf();
void scf();

void jmp();
void jmp_nz();
void jmp_z();
void jmp_nc();
void jmp_c();

void jmpr();
void jmpr_nz();
void jmpr_z();
void jmpr_nc();
void jmpr_c();

void jmp_x();

void call();
void call_nz();
void call_z();
void call_nc();
void call_c();

void ret();
void ret_nz();
void ret_z();
void ret_nc();
void ret_c();

void rst_00();
void rst_08();
void rst_10();
void rst_18();
void rst_20();
void rst_28();
void rst_30();
void rst_38();

void cb();

void rlc_b();
void rlc_c();
void rlc_d();
void rlc_e();
void rlc_h();
void rlc_l();
void rlc_x();
void rlc_a();

void rrc_b();
void rrc_c();
void rrc_d();
void rrc_e();
void rrc_h();
void rrc_l();
void rrc_x();
void rrc_a();

void rl_b();
void rl_c();
void rl_d();
void rl_e();
void rl_h();
void rl_l();
void rl_x();
void rl_a();

void rr_b();
void rr_c();
void rr_d();
void rr_e();
void rr_h();
void rr_l();
void rr_x();
void rr_a();

void sla_b();
void sla_c();
void sla_d();
void sla_e();
void sla_h();
void sla_l();
void sla_x();
void sla_a();

void sra_b();
void sra_c();
void sra_d();
void sra_e();
void sra_h();
void sra_l();
void sra_x();
void sra_a();

void swap_b();
void swap_c();
void swap_d();
void swap_e();
void swap_h();
void swap_l();
void swap_x();
void swap_a();

void srl_b();
void srl_c();
void srl_d();
void srl_e();
void srl_h();
void srl_l();
void srl_x();
void srl_a();

void bit0_b();
void bit0_c();
void bit0_d();
void bit0_e();
void bit0_h();
void bit0_l();
void bit0_x();
void bit0_a();

void bit1_b();
void bit1_c();
void bit1_d();
void bit1_e();
void bit1_h();
void bit1_l();
void bit1_x();
void bit1_a();

void bit2_b();
void bit2_c();
void bit2_d();
void bit2_e();
void bit2_h();
void bit2_l();
void bit2_x();
void bit2_a();

void bit3_b();
void bit3_c();
void bit3_d();
void bit3_e();
void bit3_h();
void bit3_l();
void bit3_x();
void bit3_a();

void bit4_b();
void bit4_c();
void bit4_d();
void bit4_e();
void bit4_h();
void bit4_l();
void bit4_x();
void bit4_a();

void bit5_b();
void bit5_c();
void bit5_d();
void bit5_e();
void bit5_h();
void bit5_l();
void bit5_x();
void bit5_a();

void bit6_b();
void bit6_c();
void bit6_d();
void bit6_e();
void bit6_h();
void bit6_l();
void bit6_x();
void bit6_a();

void bit7_b();
void bit7_c();
void bit7_d();
void bit7_e();
void bit7_h();
void bit7_l();
void bit7_x();
void bit7_a();

void res0_b();
void res0_c();
void res0_d();
void res0_e();
void res0_h();
void res0_l();
void res0_x();
void res0_a();

void res1_b();
void res1_c();
void res1_d();
void res1_e();
void res1_h();
void res1_l();
void res1_x();
void res1_a();

void res2_b();
void res2_c();
void res2_d();
void res2_e();
void res2_h();
void res2_l();
void res2_x();
void res2_a();

void res3_b();
void res3_c();
void res3_d();
void res3_e();
void res3_h();
void res3_l();
void res3_x();
void res3_a();

void res4_b();
void res4_c();
void res4_d();
void res4_e();
void res4_h();
void res4_l();
void res4_x();
void res4_a();

void res5_b();
void res5_c();
void res5_d();
void res5_e();
void res5_h();
void res5_l();
void res5_x();
void res5_a();

void res6_b();
void res6_c();
void res6_d();
void res6_e();
void res6_h();
void res6_l();
void res6_x();
void res6_a();

void res7_b();
void res7_c();
void res7_d();
void res7_e();
void res7_h();
void res7_l();
void res7_x();
void res7_a();

void set0_b();
void set0_c();
void set0_d();
void set0_e();
void set0_h();
void set0_l();
void set0_x();
void set0_a();

void set1_b();
void set1_c();
void set1_d();
void set1_e();
void set1_h();
void set1_l();
void set1_x();
void set1_a();

void set2_b();
void set2_c();
void set2_d();
void set2_e();
void set2_h();
void set2_l();
void set2_x();
void set2_a();

void set3_b();
void set3_c();
void set3_d();
void set3_e();
void set3_h();
void set3_l();
void set3_x();
void set3_a();

void set4_b();
void set4_c();
void set4_d();
void set4_e();
void set4_h();
void set4_l();
void set4_x();
void set4_a();

void set5_b();
void set5_c();
void set5_d();
void set5_e();
void set5_h();
void set5_l();
void set5_x();
void set5_a();

void set6_b();
void set6_c();
void set6_d();
void set6_e();
void set6_h();
void set6_l();
void set6_x();
void set6_a();

void set7_b();
void set7_c();
void set7_d();
void set7_e();
void set7_h();
void set7_l();
void set7_x();
void set7_a();
