#include "../src/cpu.h"
#include "../src/debug.h"
#include "../src/mem.h"
#include "../src/timer.h"
#include "unity.h"
#include "unity_internals.h"

typedef struct {
    char name[16];
    u8 ticks_expected;
} TestCase;

TestCase test_cases[0x100] = {
    // 0x00
    { "nop", 4 },
    { "ld_bc_nn", 12 },
    { "ld_bc_a", 8 },
    { "inc16_bc", 8 },
    { "inc_b", 4 },
    { "dec_b", 4 },
    { "ld_bn", 8 },
    { "rotlc_a", 4 },

    // 0x08
    { "ld_nn_sp", 20 },
    { "add16_bc", 8 },
    { "ld_a_bc", 8 },
    { "dec16_bc", 8 },
    { "inc_c", 4 },
    { "dec_c", 4 },
    { "ld_cn", 8 },
    { "rotrc_a", 4 },

    // 0x10
    { "stop", 4 },
    { "ld_de_nn", 12 },
    { "ld_de_a", 8 },
    { "inc16_de", 8 },
    { "inc_d", 4 },
    { "dec_d", 4 },
    { "ld_dn", 8 },
    { "rotl_a", 4 },

    // 0x18
    { "jmpr", 12 },
    { "add16_de", 8 },
    { "ld_a_de", 8 },
    { "dec16_de", 8 },
    { "inc_e", 4 },
    { "dec_e", 4 },
    { "ld_en", 8 },
    { "rotr_a", 4 },

    // 0x20
    { "jmpr_nz", 12 },
    { "ld_hl_nn", 12 },
    { "ldi_xa", 8 },
    { "inc16_hl", 8 },
    { "inc_h", 4 },
    { "dec_h", 4 },
    { "ld_hn", 8 },
    { "dec_adj_a", 4 },

    // 0x28
    { "jmpr_z", 8 },
    { "add16_hl", 8 },
    { "ldi_ax", 8 },
    { "dec16_hl", 8 },
    { "inc_l", 4 },
    { "dec_l", 4 },
    { "ld_ln", 8 },
    { "cmpl_a", 4 },

    // 0x30
    { "jmpr_nc", 12 },
    { "ld_sp_nn", 12 },
    { "ldd_xa", 8 },
    { "inc16_sp", 8 },
    { "inc_x", 12 },
    { "dec_x", 12 },
    { "ld_xn", 12 },
    { "scf", 4 },

    // 0x38
    { "jmpr_c", 8 },
    { "add16_sp", 8 },
    { "ldd_ax", 8 },
    { "dec16_sp", 8 },
    { "inc_a", 4 },
    { "dec_a", 4 },
    { "ld_an", 8 },
    { "ccf", 4 },

    // 0x40
    { "ld_bb", 4 },
    { "ld_bc", 4 },
    { "ld_bd", 4 },
    { "ld_be", 4 },
    { "ld_bh", 4 },
    { "ld_bl", 4 },
    { "ld_bx", 8 },
    { "ld_ba", 4 },

    // 0x48
    { "ld_cb", 4 },
    { "ld_cc", 4 },
    { "ld_cd", 4 },
    { "ld_ce", 4 },
    { "ld_ch", 4 },
    { "ld_cl", 4 },
    { "ld_cx", 8 },
    { "ld_ca", 4 },

    // 0x50
    { "ld_db", 4 },
    { "ld_dc", 4 },
    { "ld_dd", 4 },
    { "ld_de", 4 },
    { "ld_dh", 4 },
    { "ld_dl", 4 },
    { "ld_dx", 8 },
    { "ld_da", 4 },

    // 0x58
    { "ld_eb", 4 },
    { "ld_ec", 4 },
    { "ld_ed", 4 },
    { "ld_ee", 4 },
    { "ld_eh", 4 },
    { "ld_el", 4 },
    { "ld_ex", 8 },
    { "ld_ea", 4 },

    // 0x60
    { "ld_hb", 4 },
    { "ld_hc", 4 },
    { "ld_hd", 4 },
    { "ld_he", 4 },
    { "ld_hh", 4 },
    { "ld_hl", 4 },
    { "ld_hx", 8 },
    { "ld_ha", 4 },

    // 0x68
    { "ld_lb", 4 },
    { "ld_lc", 4 },
    { "ld_ld", 4 },
    { "ld_le", 4 },
    { "ld_lh", 4 },
    { "ld_ll", 4 },
    { "ld_lx", 8 },
    { "ld_la", 4 },

    // 0x70
    { "ld_xb", 8 },
    { "ld_xc", 8 },
    { "ld_xd", 8 },
    { "ld_xe", 8 },
    { "ld_xh", 8 },
    { "ld_xl", 8 },
    { "halt", 4 },
    { "ld_xa", 8 },

    // 0x78
    { "ld_ab", 4 },
    { "ld_ac", 4 },
    { "ld_ad", 4 },
    { "ld_ae", 4 },
    { "ld_ah", 4 },
    { "ld_al", 4 },
    { "ld_ax", 8 },
    { "ld_aa", 4 },

    // 0x80
    { "add_ab", 4 },
    { "add_ac", 4 },
    { "add_ad", 4 },
    { "add_ae", 4 },
    { "add_ah", 4 },
    { "add_al", 4 },
    { "add_ax", 8 },
    { "add_aa", 4 },

    // 0x88
    { "adc_ab", 4 },
    { "adc_ac", 4 },
    { "adc_ad", 4 },
    { "adc_ae", 4 },
    { "adc_ah", 4 },
    { "adc_al", 4 },
    { "adc_ax", 8 },
    { "adc_aa", 4 },

    // 0x90
    { "sub_ab", 4 },
    { "sub_ac", 4 },
    { "sub_ad", 4 },
    { "sub_ae", 4 },
    { "sub_ah", 4 },
    { "sub_al", 4 },
    { "sub_ax", 8 },
    { "sub_aa", 4 },

    // 0x98
    { "sbc_ab", 4 },
    { "sbc_ac", 4 },
    { "sbc_ad", 4 },
    { "sbc_ae", 4 },
    { "sbc_ah", 4 },
    { "sbc_al", 4 },
    { "sbc_ax", 8 },
    { "sbc_aa", 4 },

    // 0xA0
    { "and_ab", 4 },
    { "and_ac", 4 },
    { "and_ad", 4 },
    { "and_ae", 4 },
    { "and_ah", 4 },
    { "and_al", 4 },
    { "and_ax", 8 },
    { "and_aa", 4 },

    // 0xA8
    { "xor_ab", 4 },
    { "xor_ac", 4 },
    { "xor_ad", 4 },
    { "xor_ae", 4 },
    { "xor_ah", 4 },
    { "xor_al", 4 },
    { "xor_ax", 8 },
    { "xor_aa", 4 },

    // 0xB0
    { "or_ab", 4 },
    { "or_ac", 4 },
    { "or_ad", 4 },
    { "or_ae", 4 },
    { "or_ah", 4 },
    { "or_al", 4 },
    { "or_ax", 8 },
    { "or_aa", 4 },

    // 0xB8
    { "cmp_ab", 4 },
    { "cmp_ac", 4 },
    { "cmp_ad", 4 },
    { "cmp_ae", 4 },
    { "cmp_ah", 4 },
    { "cmp_al", 4 },
    { "cmp_ax", 8 },
    { "cmp_aa", 4 },

    // 0xC0
    { "ret_nz", 20 },
    { "pop_bc", 12 },
    { "jmp_nz", 16 },
    { "jmp", 16 },
    { "call_nz", 24 },
    { "push_bc", 16 },
    { "add_an", 8 },
    { "rst_00", 16 },

    // 0xC8
    { "ret_z", 8 },
    { "ret", 16 },
    { "jmp_z", 12 },
    { "cb", 8 },
    { "call_z", 12 },
    { "call", 24 },
    { "adc_an", 8 },
    { "rst_08", 16 },

    // 0xD0
    { "ret_nc", 20 },
    { "pop_de", 12 },
    { "jmp_nc", 16 },
    { "nop", 4 },
    { "call_nc", 24 },
    { "push_de", 16 },
    { "sub_an", 8 },
    { "rst_10", 16 },

    // 0xD8
    { "ret_c", 8 },
    { "ret_i", 16 },
    { "jmp_c", 12 },
    { "nop", 4 },
    { "call_c", 12 },
    { "nop", 4 },
    { "sbc_an", 8 },
    { "rst_18", 16 },

    // 0xE0
    { "ldio_n_a", 12 },
    { "pop_hl", 12 },
    { "ldio_c_a", 8 },
    { "nop", 4 },
    { "nop", 4 },
    { "push_hl", 16 },
    { "and_an", 8 },
    { "rst_20", 16 },

    // 0xE8
    { "add_sp_n", 16 },
    { "jmp_x", 4 },
    { "ld_nn_a", 16 },
    { "nop", 4 },
    { "nop", 4 },
    { "nop", 4 },
    { "xor_an", 8 },
    { "rst_28", 16 },

    // 0xF0
    { "ldio_a_n", 12 },
    { "pop_af", 12 },
    { "ldio_a_c", 8 },
    { "di", 4 },
    { "nop", 4 },
    { "push_af", 16 },
    { "or_an", 8 },
    { "rst_30", 16 },

    // 0xF8
    { "ld_hl_sp_n", 12 },
    { "ld_sp_hl", 8 },
    { "ld_a_nn", 16 },
    { "ei", 4 },
    { "nop", 4 },
    { "nop", 4 },
    { "cmp_an", 8 },
    { "rst_38", 16 },
};

u8 code = 0;
char message[32];

void test_instruction() {
    TestCase tc = test_cases[code];
    sprintf(message, "(name: %s, code: 0x%02X)", tc.name, code);

    mem[cpu.reg.pc] = code;
    cpu_step();

    TEST_ASSERT_EQUAL_MESSAGE(tc.ticks_expected, ticks, message);
}

void setUp() {
    cpu_init();
    set_flags(0, 0, 0, 0);
    ticks = 0;
}

void tearDown() {}

int main(void) {
    UNITY_BEGIN();

    for (int i = 0; i < 0x100; i++) {
        if (i == 0xCB) continue;
        code = i;
        RUN_TEST(test_instruction);
    }

    return UNITY_END();
}
