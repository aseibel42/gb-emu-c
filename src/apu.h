#pragma once

#include "util.h"

void apu_init();
void apu_tick();

void frame_sequence_tick();
void length_counter_tick();
void volume_envelope_tick();
void sweep_tick();

void ch2_step();
void ch2_trigger();

void apu_set_period();
u8 apu_get_wave_bit(u8 wave_pattern, u8 wave_bit);

void apu_set_cur_vol();
void apu_set_len();
void apu_set_ch2_dac_enabled(bool flag);


