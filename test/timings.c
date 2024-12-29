#include "../src/instruction.h"
#include "../src/cpu.h"
#include "../src/emu.h"
#include "../src/mem.h"
#include "unity.h"
#include "unity_internals.h"

typedef struct {
    Instruction instruction;
    u8 ticks_expected;
} TestCase;

TestCase test_cases[0x100] = {
    // 0x00
    { nop, 0 },
    { ld_bc_nn, 0 },
    { ld_bc_a, 0 },
    { inc16_bc, 0 },
    { inc_b, 0 },
    { dec_b, 0 },
    { ld_bn, 0 },
    { rotlc_a, 0 },

    // 0x08
    { ld_nn_sp, 0 },
    { add16_bc, 0 },
    { ld_a_bc, 0 },
    { dec16_bc, 0 },
    { inc_c, 0 },
    { dec_c, 0 },
    { ld_cn, 0 },
    { rotrc_a, 0 },

    // 0x10
    { stop, 0 },
    { ld_de_nn, 0 },
    { ld_de_a, 0 },
    { inc16_de, 0 },
    { inc_d, 0 },
    { dec_d, 0 },
    { ld_dn, 0 },
    { rotl_a, 0 },

    // 0x18
    { jmpr, 0 },
    { add16_de, 0 },
    { ld_a_de, 0 },
    { dec16_de, 0 },
    { inc_e, 0 },
    { dec_e, 0 },
    { ld_en, 0 },
    { rotr_a, 0 },

    // 0x20
    { jmpr_nz, 0 },
    { ld_hl_nn, 0 },
    { ldi_xa, 0 },
    { inc16_hl, 0 },
    { inc_h, 0 },
    { dec_h, 0 },
    { ld_hn, 0 },
    { dec_adj_a, 0 },

    // 0x28
    { jmpr_z, 0 },
    { add16_hl, 0 },
    { ldi_ax, 0 },
    { dec16_hl, 0 },
    { inc_l, 0 },
    { dec_l, 0 },
    { ld_ln, 0 },
    { cmpl_a, 0 },

    // 0x30
    { jmpr_nc, 0 },
    { ld_sp_nn, 0 },
    { ldd_xa, 0 },
    { inc16_sp, 0 },
    { inc_x, 0 },
    { dec_x, 0 },
    { ld_xn, 0 },
    { scf, 0 },

    // 0x38
    { jmpr_c, 0 },
    { add16_sp, 0 },
    { ldd_ax, 0 },
    { dec16_sp, 0 },
    { inc_a, 0 },
    { dec_a, 0 },
    { ld_an, 0 },
    { ccf, 0 },

    // 0x40
    { ld_bb, 0 },
    { ld_bc, 0 },
    { ld_bd, 0 },
    { ld_be, 0 },
    { ld_bh, 0 },
    { ld_bl, 0 },
    { ld_bx, 0 },
    { ld_ba, 0 },

    // 0x48
    { ld_cb, 0 },
    { ld_cc, 0 },
    { ld_cd, 0 },
    { ld_ce, 0 },
    { ld_ch, 0 },
    { ld_cl, 0 },
    { ld_cx, 0 },
    { ld_ca, 0 },

    // 0x50
    { ld_db, 0 },
    { ld_dc, 0 },
    { ld_dd, 0 },
    { ld_de, 0 },
    { ld_dh, 0 },
    { ld_dl, 0 },
    { ld_dx, 0 },
    { ld_da, 0 },

    // 0x58
    { ld_eb, 0 },
    { ld_ec, 0 },
    { ld_ed, 0 },
    { ld_ee, 0 },
    { ld_eh, 0 },
    { ld_el, 0 },
    { ld_ex, 0 },
    { ld_ea, 0 },

    // 0x60
    { ld_hb, 0 },
    { ld_hc, 0 },
    { ld_hd, 0 },
    { ld_he, 0 },
    { ld_hh, 0 },
    { ld_hl, 0 },
    { ld_hx, 0 },
    { ld_ha, 0 },

    // 0x68
    { ld_lb, 0 },
    { ld_lc, 0 },
    { ld_ld, 0 },
    { ld_le, 0 },
    { ld_lh, 0 },
    { ld_ll, 0 },
    { ld_lx, 0 },
    { ld_la, 0 },

    // 0x70
    { ld_xb, 0 },
    { ld_xc, 0 },
    { ld_xd, 0 },
    { ld_xe, 0 },
    { ld_xh, 0 },
    { ld_xl, 0 },
    { halt, 0 },
    { ld_xa, 0 },

    // 0x78
    { ld_ab, 0 },
    { ld_ac, 0 },
    { ld_ad, 0 },
    { ld_ae, 0 },
    { ld_ah, 0 },
    { ld_al, 0 },
    { ld_ax, 0 },
    { ld_aa, 0 },

    // 0x80
    { add_ab, 0 },
    { add_ac, 0 },
    { add_ad, 0 },
    { add_ae, 0 },
    { add_ah, 0 },
    { add_al, 0 },
    { add_ax, 0 },
    { add_aa, 0 },

    // 0x88
    { adc_ab, 0 },
    { adc_ac, 0 },
    { adc_ad, 0 },
    { adc_ae, 0 },
    { adc_ah, 0 },
    { adc_al, 0 },
    { adc_ax, 0 },
    { adc_aa, 0 },

    // 0x90
    { sub_ab, 0 },
    { sub_ac, 0 },
    { sub_ad, 0 },
    { sub_ae, 0 },
    { sub_ah, 0 },
    { sub_al, 0 },
    { sub_ax, 0 },
    { sub_aa, 0 },

    // 0x98
    { sbc_ab, 0 },
    { sbc_ac, 0 },
    { sbc_ad, 0 },
    { sbc_ae, 0 },
    { sbc_ah, 0 },
    { sbc_al, 0 },
    { sbc_ax, 0 },
    { sbc_aa, 0 },

    // 0xA0
    { and_ab, 0 },
    { and_ac, 0 },
    { and_ad, 0 },
    { and_ae, 0 },
    { and_ah, 0 },
    { and_al, 0 },
    { and_ax, 0 },
    { and_aa, 0 },

    // 0xA8
    { xor_ab, 0 },
    { xor_ac, 0 },
    { xor_ad, 0 },
    { xor_ae, 0 },
    { xor_ah, 0 },
    { xor_al, 0 },
    { xor_ax, 0 },
    { xor_aa, 0 },

    // 0xB0
    { or_ab, 0 },
    { or_ac, 0 },
    { or_ad, 0 },
    { or_ae, 0 },
    { or_ah, 0 },
    { or_al, 0 },
    { or_ax, 0 },
    { or_aa, 0 },

    // 0xB8
    { cmp_ab, 0 },
    { cmp_ac, 0 },
    { cmp_ad, 0 },
    { cmp_ae, 0 },
    { cmp_ah, 0 },
    { cmp_al, 0 },
    { cmp_ax, 0 },
    { cmp_aa, 0 },

    // 0xC0
    { ret_nz, 0 },
    { pop_bc, 0 },
    { jmp_nz, 0 },
    { jmp, 0 },
    { call_nz, 0 },
    { push_bc, 0 },
    { add_an, 0 },
    { rst_00, 0 },

    // 0xC8
    { ret_z, 0 },
    { ret, 0 },
    { jmp_z, 0 },
    { nop, 0 },
    { call_z, 0 },
    { call, 0 },
    { adc_an, 0 },
    { rst_08, 0 },

    // 0xD0
    { ret_nc, 0 },
    { pop_de, 0 },
    { jmp_nc, 0 },
    { nop, 0 },
    { call_nc, 0 },
    { push_de, 0 },
    { sub_an, 0 },
    { rst_10, 0 },

    // 0xD8
    { ret_c, 0 },
    { ret_i, 0 },
    { jmp_c, 0 },
    { nop, 0 },
    { call_c, 0 },
    { nop, 0 },
    { sbc_an, 0 },
    { rst_18, 0 },

    // 0xE0
    { ldio_n_a, 0 },
    { pop_hl, 0 },
    { ldio_c_a, 0 },
    { nop, 0 },
    { nop, 0 },
    { push_hl, 0 },
    { and_an, 0 },
    { rst_20, 0 },

    // 0xE8
    { add_sp_n, 0 },
    { jmp_x, 0 },
    { ld_nn_a, 0 },
    { nop, 0 },
    { nop, 0 },
    { nop, 0 },
    { xor_an, 0 },
    { rst_28, 0 },

    // 0xF0
    { ldio_a_n, 0 },
    { pop_af, 0 },
    { ldio_a_c, 0 },
    { di, 0 },
    { nop, 0 },
    { push_af, 0 },
    { or_an, 0 },
    { rst_30, 0 },

    // 0xF8
    { ld_hl_sp_n, 0 },
    { ld_sp_hl, 0 },
    { ld_a_nn, 0 },
    { ei, 0 },
    { nop, 0 },
    { nop, 0 },
    { cmp_an, 0 },
    { rst_38, 0 },
};

void setUp() {
    cpu_init();
    ticks = 0;
}

void tearDown() {

}

void test_instruction_timings() {
    char message[4];
    for (int i = 0; i < 0x100; i++) {
        if (i == 0xCB) continue;
        TestCase tc = test_cases[i];
        sprintf(message, "x%02x", i);
        TEST_ASSERT_EQUAL_MESSAGE(ticks, tc.ticks_expected, message);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_instruction_timings);
    return UNITY_END();
}
