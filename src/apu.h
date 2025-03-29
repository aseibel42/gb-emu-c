#pragma once

#include "util.h"
#include "ui.h"
#include "io.h"

#define M_CYCLES_PER_FRAME 17556
#define SOURCE_SAMPLE_RATE (M_CYCLES_PER_FRAME / 4 * (int)MAX_FPS) //263340
#define TARGET_SAMPLE_RATE 48000   // Playback sample rate (Hz)
#define SOURCE_BUFFER_SIZE 4389          // Number of source audio samples per frames
#define TARGET_FRAMES (((int)SOURCE_BUFFER_SIZE * TARGET_SAMPLE_RATE) / SOURCE_SAMPLE_RATE) 

typedef union {
    struct {
        float prev[TARGET_FRAMES * 2];
        float new[TARGET_FRAMES * 2];
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

    float *source_sample_buffer;
    AudioBuffer *target_sample_buffer;
    float *render_buffer;

    // channel-specific values and pointers (referencing channel 1 or 2 registers)
    u8 master_ctrl_bit;    
    ch_vol *ch_vol;
    u8 *ch_freq;
    ch_ctrl *ch_ctrl;
} SquareChannel;

// general
void apu_init();
void apu_tick();

void frame_sequence_tick();
void period_counter_tick();
void square_period_counter_tick(SquareChannel *ch);
void length_counter_tick();
void square_length_counter_tick(SquareChannel *ch);
void volume_envelope_tick();
void square_vol_env_tick(SquareChannel *ch);
void sweep_tick();

u8 apu_get_wave_bit(u8 wave_pattern, u8 wave_bit);

// ch1
void ch1_trigger();
void ch1_apu_set_period();
void ch1_apu_set_cur_vol();
void ch1_apu_set_len();
void apu_set_ch1_dac_enabled(bool flag);
void apu_set_ch1_env_enabled(bool flag);
void apu_reset_ch1_env_counter();

// ch2
void ch2_trigger();
void ch2_apu_set_period();
void ch2_apu_set_cur_vol();
void ch2_apu_set_len();
void apu_set_ch2_dac_enabled(bool flag);
void apu_set_ch2_env_enabled(bool flag);
void apu_reset_ch2_env_counter();

// audio buffer
void generate_source_audio_samples();
void resample_audio();
void queue_audio();

int find_trigger_point(float buffer[]);
void shift_waveform(int trigger_index, float combined_buffer[], float render_buffer[]);
void mix_buffers(const float *ch1, const float *ch2, float *result);

SquareChannel init_square_channel(u8 ch_num);
