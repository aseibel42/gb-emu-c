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

// general
void apu_init();
SquareChannel init_square_channel(u8 ch_num);
void apu_tick();

void period_counter_tick();
void square_period_counter_tick(SquareChannel *ch);

void frame_sequence_tick();
void length_counter_tick();
void square_length_counter_tick(SquareChannel *ch);
void volume_envelope_tick();
void square_vol_env_tick(SquareChannel *ch);
void sweep_tick();

void square_trigger(SquareChannel *ch);
void square_handle_volume_write(SquareChannel *ch);

// audio buffer
void generate_source_audio_samples(SquareChannel *ch);
void resample_audio();
void queue_audio();

u16 find_trigger_point(float buffer[]);
void mix_buffers(const float *ch1, const float *ch2, float *result);
