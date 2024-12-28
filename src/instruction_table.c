#include "instruction.h"

Instruction op[0x100] = {
    // 0x00
    nop,
    ld_bc_nn,
    ld_bc_a,
    inc16_bc,
    inc_b,
    dec_b,
    ld_bn,
    nop,

    // 0x08
    ld_nn_sp,
    add16_bc,
    ld_a_bc,
    dec16_bc,
    inc_c,
    dec_c,
    ld_cn,
    nop,

    // 0x10
    nop,
    ld_de_nn,
    ld_de_a,
    inc16_de,
    inc_d,
    dec_d,
    ld_dn,
    nop,

    // 0x18
    nop,
    add16_de,
    ld_a_de,
    dec16_de,
    inc_e,
    dec_e,
    ld_en,
    nop,

    // 0x20
    nop,
    ld_hl_nn,
    ldi_xa,
    inc16_hl,
    inc_h,
    dec_h,
    ld_hn,
    nop,

    // 0x28
    nop,
    add16_hl,
    ldi_ax,
    dec16_hl,
    inc_l,
    dec_l,
    ld_ln,
    nop,

    // 0x30
    nop,
    ld_sp_nn,
    ldd_xa,
    inc16_sp,
    inc_x,
    dec_x,
    ld_xn,
    nop,

    // 0x38
    nop,
    add16_sp,
    ldd_ax,
    dec16_sp,
    inc_a,
    dec_a,
    ld_an,
    nop,

    // 0x40
    ld_bb,
    ld_bc,
    ld_bd,
    ld_be,
    ld_bh,
    ld_bl,
    ld_bx,
    ld_ba,

    // 0x48
    ld_cb,
    ld_cc,
    ld_cd,
    ld_ce,
    ld_ch,
    ld_cl,
    ld_cx,
    ld_ca,

    // 0x50
    ld_db,
    ld_dc,
    ld_dd,
    ld_de,
    ld_dh,
    ld_dl,
    ld_dx,
    ld_da,

    // 0x58
    ld_eb,
    ld_ec,
    ld_ed,
    ld_ee,
    ld_eh,
    ld_el,
    ld_ex,
    ld_ea,

    // 0x60
    ld_hb,
    ld_hc,
    ld_hd,
    ld_he,
    ld_hh,
    ld_hl,
    ld_hx,
    ld_ha,

    // 0x68
    ld_lb,
    ld_lc,
    ld_ld,
    ld_le,
    ld_lh,
    ld_ll,
    ld_lx,
    ld_la,

    // 0x70
    ld_xb,
    ld_xc,
    ld_xd,
    ld_xe,
    ld_xh,
    ld_xl,
    halt,
    ld_xa,

    // 0x78
    ld_ab,
    ld_ac,
    ld_ad,
    ld_ae,
    ld_ah,
    ld_al,
    ld_ax,
    ld_aa,

    // 0x80
    add_ab,
    add_ac,
    add_ad,
    add_ae,
    add_ah,
    add_al,
    add_ax,
    add_aa,

    // 0x88
    adc_ab,
    adc_ac,
    adc_ad,
    adc_ae,
    adc_ah,
    adc_al,
    adc_ax,
    adc_aa,

    // 0x90
    sub_ab,
    sub_ac,
    sub_ad,
    sub_ae,
    sub_ah,
    sub_al,
    sub_ax,
    sub_aa,

    // 0x98
    sbc_ab,
    sbc_ac,
    sbc_ad,
    sbc_ae,
    sbc_ah,
    sbc_al,
    sbc_ax,
    sbc_aa,

    // 0xA0
    and_ab,
    and_ac,
    and_ad,
    and_ae,
    and_ah,
    and_al,
    and_ax,
    and_aa,

    // 0xA8
    xor_ab,
    xor_ac,
    xor_ad,
    xor_ae,
    xor_ah,
    xor_al,
    xor_ax,
    xor_aa,

    // 0xB0
    or_ab,
    or_ac,
    or_ad,
    or_ae,
    or_ah,
    or_al,
    or_ax,
    or_aa,

    // 0xB8
    cmp_ab,
    cmp_ac,
    cmp_ad,
    cmp_ae,
    cmp_ah,
    cmp_al,
    cmp_ax,
    cmp_aa,

    // 0xC0
    nop,
    pop_bc,
    nop,
    nop,
    nop,
    push_bc,
    add_an,
    nop,

    // 0xC8
    nop,
    nop,
    nop,
    nop,
    nop,
    nop,
    adc_an,
    nop,

    // 0xD0
    nop,
    pop_de,
    nop,
    nop,
    nop,
    push_de,
    sub_an,
    nop,

    // 0xD8
    nop,
    nop,
    nop,
    nop,
    nop,
    nop,
    sbc_an,
    nop,

    // 0xE0
    ldio_n_a,
    pop_hl,
    ldio_c_a,
    nop,
    nop,
    push_hl,
    and_an,
    nop,

    // 0xE8
    add_sp_n,
    nop,
    ld_nn_a,
    nop,
    nop,
    nop,
    xor_an,
    nop,

    // 0xF0
    ldio_a_n,
    pop_af,
    ldio_a_c,
    nop,
    nop,
    push_af,
    or_an,
    nop,

    // 0xF8
    ld_hl_sp_n,
    ld_sp_hl,
    ld_a_nn,
    nop,
    nop,
    nop,
    cmp_an,
    nop,
};
