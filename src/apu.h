#pragma once

#include "io.h"
#include "ui.h"
#include "util.h"


#define M_CYCLES_PER_FRAME 17556
#define SOURCE_SAMPLE_RATE (M_CYCLES_PER_FRAME / 4 * (int)MAX_FPS) // 263340
#define TARGET_SAMPLE_RATE 48000 // Playback sample rate (Hz)
#define SOURCE_BUFFER_SIZE 4389  // Number of source audio samples per frames
#define TARGET_FRAMES (((int)SOURCE_BUFFER_SIZE * TARGET_SAMPLE_RATE) / SOURCE_SAMPLE_RATE)

typedef union {
    struct {
        float prev[TARGET_FRAMES * 2];
        float curr[TARGET_FRAMES * 2];
    };
    float combined[TARGET_FRAMES * 4];
} AudioBuffer;

// Square Channel struct
typedef struct {
    u16 period_counter;
    u8 length_counter;
    u8 vol_env_counter;
    u8 wave_duty_bit_counter;

    u8 current_vol;
    bool dac_enable;
    bool vol_env_enable;

    u8 sweep_env_counter;
    bool sweep_env_enable;

    float *source_sample_buffer;
    AudioBuffer *target_sample_buffer;
    u16 trigger_index;

    // channel-specific values and pointers (referencing channel 1 or 2 registers)
    u8 master_ctrl_bit;
    u8 master_vol_left_bit;
    u8 master_vol_right_bit;
    u8 *ch_length;
    ch_vol *ch_vol;
    u8 *ch_freq;
    ch_ctrl *ch_ctrl;
} SquareChannel;

// Wave Channel struct
typedef struct {
    u16 period_counter;
    u16 length_counter;
    u8 wave_duty_bit_counter;

    u8 current_vol;
    bool dac_enable;

    float *source_sample_buffer;
    AudioBuffer *target_sample_buffer;
    u16 trigger_index;
} WaveChannel;

// Noise Channel struct
typedef struct {
    u32 period_counter;
    u8 length_counter;
    u8 vol_env_counter;

    u8 current_vol;
    bool dac_enable;
    bool vol_env_enable;

    u16 lfsr;

    float *source_sample_buffer;
    AudioBuffer *target_sample_buffer;
    u16 trigger_index;
} NoiseChannel;

// general
void apu_init();
SquareChannel init_square_channel(u8 ch_num);
WaveChannel init_wave_channel();
NoiseChannel init_noise_channel();
void apu_tick();

void period_counter_tick();
void square_period_counter_tick(SquareChannel *ch);
void wave_period_counter_tick();
void noise_period_counter_tick();

void frame_sequence_tick();
void length_counter_tick();
void square_length_counter_tick(SquareChannel *ch);
void wave_length_counter_tick();
void noise_length_counter_tick();
void volume_envelope_tick();
void square_vol_env_tick(SquareChannel *ch);
void noise_vol_env_tick();
void sweep_tick();

void square_trigger(SquareChannel *ch);
void ch1_sweep_trigger();
void ch3_trigger();
void noise_trigger();
void square_handle_volume_write(SquareChannel *ch);
void noise_handle_volume_write();

// audio buffer
void generate_source_audio_samples(SquareChannel *ch);
void ch3_generate_source_audio_samples();
u8 ch3_get_wave_nibble(u8 index);
void ch4_generate_source_audio_samples();
void resample_audio();
void queue_audio();

u16 find_trigger_point(float buffer[]);
u16 ch3_find_trigger_point(float buffer[]);
u16 ch3_find_trigger_min(float buffer[]);
void mix_buffers(const float *ch1, const float *ch2, const float *ch3, const float *ch4, float *result);
