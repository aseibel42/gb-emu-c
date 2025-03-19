#pragma once

#include "util.h"

#define SOURCE_SAMPLE_RATE 262144  // High sample rate (Hz)
#define TARGET_SAMPLE_RATE 48000   // Playback sample rate (Hz)
#define SOURCE_BUFFER_SIZE 4389          // Number of source audio samples per frames

// general
void apu_init();
void apu_tick();

void frame_sequence_tick();
void length_counter_tick();
void volume_envelope_tick();
void sweep_tick();

u8 apu_get_wave_bit(u8 wave_pattern, u8 wave_bit);

// ch2
void ch2_trigger();
void apu_set_period();
void apu_set_cur_vol();
void apu_set_len();
void apu_set_ch2_dac_enabled(bool flag);
void apu_set_ch2_env_enabled(bool flag);
void apu_reset_ch2_env_counter();

// audio buffer
void generate_source_audio_samples();
void resample_audio();
void queue_audio();

