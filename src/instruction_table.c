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
    rotlc_a,

    // 0x08
    ld_nn_sp,
    add16_bc,
    ld_a_bc,
    dec16_bc,
    inc_c,
    dec_c,
    ld_cn,
    rotrc_a,

    // 0x10
    stop,
    ld_de_nn,
    ld_de_a,
    inc16_de,
    inc_d,
    dec_d,
    ld_dn,
    rotl_a,

    // 0x18
    jmpr,
    add16_de,
    ld_a_de,
    dec16_de,
    inc_e,
    dec_e,
    ld_en,
    rotr_a,

    // 0x20
    jmpr_nz,
    ld_hl_nn,
    ldi_xa,
    inc16_hl,
    inc_h,
    dec_h,
    ld_hn,
    dec_adj_a,

    // 0x28
    jmpr_z,
    add16_hl,
    ldi_ax,
    dec16_hl,
    inc_l,
    dec_l,
    ld_ln,
    cmpl_a,

    // 0x30
    jmpr_nc,
    ld_sp_nn,
    ldd_xa,
    inc16_sp,
    inc_x,
    dec_x,
    ld_xn,
    scf,

    // 0x38
    jmpr_c,
    add16_sp,
    ldd_ax,
    dec16_sp,
    inc_a,
    dec_a,
    ld_an,
    ccf,

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
    ret_nz,
    pop_bc,
    jmp_nz,
    jmp,
    call_nz,
    push_bc,
    add_an,
    rst_00,

    // 0xC8
    ret_z,
    ret,
    jmp_z,
    cb,
    call_z,
    call,
    adc_an,
    rst_08,

    // 0xD0
    ret_nc,
    pop_de,
    jmp_nc,
    nop,
    call_nc,
    push_de,
    sub_an,
    rst_10,

    // 0xD8
    ret_c,
    ret_i,
    jmp_c,
    nop,
    call_c,
    nop,
    sbc_an,
    rst_18,

    // 0xE0
    ldio_n_a,
    pop_hl,
    ldio_c_a,
    nop,
    nop,
    push_hl,
    and_an,
    rst_20,

    // 0xE8
    add_sp_n,
    jmp_x,
    ld_nn_a,
    nop,
    nop,
    nop,
    xor_an,
    rst_28,

    // 0xF0
    ldio_a_n,
    pop_af,
    ldio_a_c,
    di,
    nop,
    push_af,
    or_an,
    rst_30,

    // 0xF8
    ld_hl_sp_n,
    ld_sp_hl,
    ld_a_nn,
    ei,
    nop,
    nop,
    cmp_an,
    rst_38,
};

Instruction op_cb[0x100] = {
    // 0x00
    rlc_b,
    rlc_c,
    rlc_d,
    rlc_e,
    rlc_h,
    rlc_l,
    rlc_x,
    rlc_a,

    // 0x08
    rrc_b,
    rrc_c,
    rrc_d,
    rrc_e,
    rrc_h,
    rrc_l,
    rrc_x,
    rrc_a,

    // 0x10
    rl_b,
    rl_c,
    rl_d,
    rl_e,
    rl_h,
    rl_l,
    rl_x,
    rl_a,

    // 0x18
    rr_b,
    rr_c,
    rr_d,
    rr_e,
    rr_h,
    rr_l,
    rr_x,
    rr_a,

    // 0x20
    sla_b,
    sla_c,
    sla_d,
    sla_e,
    sla_h,
    sla_l,
    sla_x,
    sla_a,

    // 0x28
    sra_b,
    sra_c,
    sra_d,
    sra_e,
    sra_h,
    sra_l,
    sra_x,
    sra_a,

    // 0x30
    swap_b,
    swap_c,
    swap_d,
    swap_e,
    swap_h,
    swap_l,
    swap_x,
    swap_a,

    // 0x38
    srl_b,
    srl_c,
    srl_d,
    srl_e,
    srl_h,
    srl_l,
    srl_x,
    srl_a,

    // 0x40
    bit0_b,
    bit0_c,
    bit0_d,
    bit0_e,
    bit0_h,
    bit0_l,
    bit0_x,
    bit0_a,

    // 0x48
    bit1_b,
    bit1_c,
    bit1_d,
    bit1_e,
    bit1_h,
    bit1_l,
    bit1_x,
    bit1_a,

    // 0x50
    bit2_b,
    bit2_c,
    bit2_d,
    bit2_e,
    bit2_h,
    bit2_l,
    bit2_x,
    bit2_a,

    // 0x58
    bit3_b,
    bit3_c,
    bit3_d,
    bit3_e,
    bit3_h,
    bit3_l,
    bit3_x,
    bit3_a,

    // 0x60
    bit4_b,
    bit4_c,
    bit4_d,
    bit4_e,
    bit4_h,
    bit4_l,
    bit4_x,
    bit4_a,

    // 0x68
    bit5_b,
    bit5_c,
    bit5_d,
    bit5_e,
    bit5_h,
    bit5_l,
    bit5_x,
    bit5_a,

    // 0x70
    bit6_b,
    bit6_c,
    bit6_d,
    bit6_e,
    bit6_h,
    bit6_l,
    bit6_x,
    bit6_a,

    // 0x78
    bit7_b,
    bit7_c,
    bit7_d,
    bit7_e,
    bit7_h,
    bit7_l,
    bit7_x,
    bit7_a,

    // 0x80
    res0_b,
    res0_c,
    res0_d,
    res0_e,
    res0_h,
    res0_l,
    res0_x,
    res0_a,

    // 0x88
    res1_b,
    res1_c,
    res1_d,
    res1_e,
    res1_h,
    res1_l,
    res1_x,
    res1_a,

    // 0x90
    res2_b,
    res2_c,
    res2_d,
    res2_e,
    res2_h,
    res2_l,
    res2_x,
    res2_a,

    // 0x98
    res3_b,
    res3_c,
    res3_d,
    res3_e,
    res3_h,
    res3_l,
    res3_x,
    res3_a,

    // 0xA0
    res4_b,
    res4_c,
    res4_d,
    res4_e,
    res4_h,
    res4_l,
    res4_x,
    res4_a,

    // 0xA8
    res5_b,
    res5_c,
    res5_d,
    res5_e,
    res5_h,
    res5_l,
    res5_x,
    res5_a,

    // 0xB0
    res6_b,
    res6_c,
    res6_d,
    res6_e,
    res6_h,
    res6_l,
    res6_x,
    res6_a,

    // 0xB8
    res7_b,
    res7_c,
    res7_d,
    res7_e,
    res7_h,
    res7_l,
    res7_x,
    res7_a,

    // 0xC0
    set0_b,
    set0_c,
    set0_d,
    set0_e,
    set0_h,
    set0_l,
    set0_x,
    set0_a,

    // 0xC8
    set1_b,
    set1_c,
    set1_d,
    set1_e,
    set1_h,
    set1_l,
    set1_x,
    set1_a,

    // 0xD0
    set2_b,
    set2_c,
    set2_d,
    set2_e,
    set2_h,
    set2_l,
    set2_x,
    set2_a,

    // 0xD8
    set3_b,
    set3_c,
    set3_d,
    set3_e,
    set3_h,
    set3_l,
    set3_x,
    set3_a,

    // 0xE0
    set4_b,
    set4_c,
    set4_d,
    set4_e,
    set4_h,
    set4_l,
    set4_x,
    set4_a,

    // 0xE8
    set5_b,
    set5_c,
    set5_d,
    set5_e,
    set5_h,
    set5_l,
    set5_x,
    set5_a,

    // 0xF0
    set6_b,
    set6_c,
    set6_d,
    set6_e,
    set6_h,
    set6_l,
    set6_x,
    set6_a,

    // 0xF8
    set7_b,
    set7_c,
    set7_d,
    set7_e,
    set7_h,
    set7_l,
    set7_x,
    set7_a,
};
