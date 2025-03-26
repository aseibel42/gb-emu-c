#include <SDL2/SDL.h>
#include <stdio.h>

#include "apu.h"
#include "io.h"

// External
extern int cpu_speed;

// General counters
u8 frame_sequence_counter = 0;
u16 period_counter = 0;
u8 sample_tick_counter = 0;
u8 wave_duty_bit = 0;

// Ch2
u8 ch2_length_counter = 0;
u8 ch2_vol_env_counter = 0;
u8 ch2_current_volume = 0;
bool ch2_dac_enabled = true;
bool ch2_env_enabled = false;

// Audio buffer
u16 source_buffer_count = 0;
u32 actual_target_frames;
float* source_sample_buffer = NULL;
float* render_buffer = NULL;
AudioBuffer target_buffer;

// Audio device
SDL_AudioSpec desired, obtained;
SDL_AudioDeviceID dev;

const u8 wave_duty_table[4] = {
    0b00000001,  // 12.5% duty cycle
    0b10000001,  // 25% duty cycle
    0b10000111,  // 50% duty cycle
    0b01111110   // 75% duty cycle
};

void apu_init() {
    // Initialize SDL for audio, return error if it fails
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
    }

    // Save audio device settings
    SDL_memset(&desired, 0, sizeof(desired));
    desired.freq = TARGET_SAMPLE_RATE; // Modern audio devices run at 48,000 Hz
    desired.format = AUDIO_F32SYS; // 32 bit floats with system endianness
    desired.channels = 2; // 2-channel stereo (L, R, L, R ...)
    desired.samples = TARGET_FRAMES; // Audio samples per frame at 48,000 Hz and 60 FPS
    desired.callback = NULL;  // No callback, manual audio handling

    // Open audio device with desired settings, return error if it fails
    dev = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    if (dev == 0) {
        printf("SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        SDL_Quit();
    }

    // Allocate heap memory for audio sample buffers
    source_sample_buffer = malloc(SOURCE_BUFFER_SIZE * 2 * sizeof(float)); // 4389 samples per frame, *2 for L and R
    render_buffer = malloc(TARGET_FRAMES * 2 * sizeof(float));
    AudioBuffer *target_buffer = malloc(sizeof(AudioBuffer)); // ~803 samples per frame after resampling from 262144 to 48000 Hz (store 2 frames worth)
    if (!target_buffer) {
        perror("Memory allocation failed");
    }

    // Zero out the target buffer
    memset(target_buffer, 0, sizeof(AudioBuffer));

    // Unpause audio device
    SDL_PauseAudioDevice(dev, 0);
}

// APU ticks once every M-cycle
void apu_tick() {
    // Increment period counter, if 2048, it gets reset to the period in NR23/24 and wavedutybit is incremented
    period_counter++;
    if(period_counter >= 2048) {
        apu_set_period(); // ghostboy says this should be (2048-period)*4
        wave_duty_bit = (wave_duty_bit + 1) & 0b111;
    }

    // Get a new sample every 4 M-ticks - 17556 M-ticks per frame at 60 FPS -> (17,556 / 32) * 8 => 4389 audio samples / frame for 256kHz sample rate
    sample_tick_counter++;
    if(sample_tick_counter == (4 * cpu_speed)) {
        generate_source_audio_samples();
        sample_tick_counter = 0; // reset counter
    }
}

// Frame sequence counter increments from 0 to 7 causing other counters to increment at certain intervals
void frame_sequence_tick() {
    frame_sequence_counter = (frame_sequence_counter + 1) & 0x7;

    // increment other counters depending on frame sequence counter's value
    if ((frame_sequence_counter & 1) == 0) {
        length_counter_tick();
    }
    if (frame_sequence_counter == 7) {
        volume_envelope_tick();
    }
    if (frame_sequence_counter == 2 || frame_sequence_counter == 6) {
        sweep_tick();
    }
}

// Call at 256 Hz - once every 16384 ticks - 4096 M-cycles
void length_counter_tick() {
    // Length counter ticks up to 64.  If it reaches 64, turn channel off
    if (ch2_length_counter < 64 && io.ch2_ctrl.length_enable) {
        ch2_length_counter++;
        if (ch2_length_counter > 63) {
            io.master_ctrl.ch2_enable = false; // turn ch2 off
        }
    } 
}

// Call at 64 Hz - once every 16384 M-cycles
void volume_envelope_tick() {
    // env should be disabled if pace = 0
    if(ch2_env_enabled && io.master_ctrl.ch2_enable) {
        ch2_vol_env_counter++;

        // Every time that vol_evn pace (1-7) is reached, change volume
        // add 1 to volume if direction bit set, sub 1 if not set
        if(ch2_vol_env_counter == io.ch2_vol.pace) {
            // Reset envelope counter once pace is reached
            ch2_vol_env_counter = 0;
            
            // If direction bit is set then crescendo, otherwise decrescendo.  Lock vol between 0 and 15
            if (io.ch2_vol.dir) {
                if (ch2_current_volume < 15) {
                    ch2_current_volume++;
                }
            } else {
                if (ch2_current_volume > 0) {
                    ch2_current_volume--;
                }
            }

            // Disable vol_env if volume reaches 0 or 15
            if (ch2_current_volume == 0 || ch2_current_volume == 15) {
                ch2_env_enabled = false;
            }          
        }
    }
}

void sweep_tick() {

}

void ch2_trigger() {
    // enable ch2 on FF26 register
    io.master_ctrl.ch2_enable = true;

    // if expired, reset length timer
    if (ch2_length_counter >= 64) {
        apu_set_len();
    }

    // reset period (set initial value to NR23/24)
    apu_set_period();

    // reset env timer
    ch2_vol_env_counter = 0;

    // reset volume
    ch2_current_volume = io.ch2_vol.init_vol;
}

void apu_set_period() {
    u16 period = ((u16)io.ch2_ctrl.period << 8) | io.ch2_freq;
    period_counter = period;
}

u8 apu_get_wave_bit(u8 wave_pattern, u8 wave_bit) {
    return (wave_duty_table[wave_pattern] >> (7 - wave_bit)) & 1;
}

void apu_set_cur_vol() {
    ch2_current_volume = io.ch2_vol.init_vol;
}

void apu_set_len() {
    ch2_length_counter = (io.ch2_len & 0b111111);
}

void apu_set_ch2_dac_enabled(bool flag) {
    ch2_dac_enabled = flag;
}

void apu_set_ch2_env_enabled(bool flag) {
    ch2_env_enabled = flag;
}

void apu_reset_ch2_env_counter() {
    ch2_vol_env_counter = 0;
}

void generate_source_audio_samples() {
    // Retrieve wave pattern from highest 2 bits of NR21
    u8 wave_pattern = (io.ch2_len >> 6) & 0b11;

    // Rescale output volume (0 -> -1, 15 -> 1)
    float output_volume = ((float)ch2_current_volume / 7.5f) - 1.0f;

    // scale volume based on master left and right volume (value of 0-7 refers to vol level of 1-8, divide by 8 to rescale between -1 and 1)
    float left_output_volume = output_volume * (io.master_vol.vol_left + 1)/8;
    float right_output_volume = output_volume * (io.master_vol.vol_right + 1)/8;

    // if channel enabled, dac enabled, and waveduty bit is 1, set output volume to calc. value, otherwise set to -1 (first L, then R)
    if (source_buffer_count < SOURCE_BUFFER_SIZE) {
        source_sample_buffer[2*source_buffer_count] = (io.master_pan.ch2_left && ch2_dac_enabled && apu_get_wave_bit(wave_pattern, wave_duty_bit)) ? left_output_volume : -1; 
        source_sample_buffer[2*source_buffer_count+1] = (io.master_pan.ch2_right && ch2_dac_enabled && apu_get_wave_bit(wave_pattern, wave_duty_bit)) ? right_output_volume : -1; 
        source_buffer_count++;
    }
}

// resample audio from source sample rate (262144 Hz) to sample rate of audio device (48000 Hz)
void resample_audio() {
    // Save new buffer as previous buffer for next iteration
    memcpy(target_buffer.prev, target_buffer.new, sizeof(float) * TARGET_FRAMES * 2);

    // find number of source frames per target frame - (262144/48000 ~ 5.46)
    float step = (float)SOURCE_SAMPLE_RATE / TARGET_SAMPLE_RATE;
    
    // calculate expected number of target frames
    actual_target_frames = source_buffer_count / step; 
 
    // calculate target audio samples by linearly interpolating between source audio samples
    float index = 0.0f; 
    for (u32 i = 0; i < actual_target_frames; i++) {
        // find relative distance of target sample from first source sample
        int int_index = (int)index;
        float frac = index - int_index;

        // find points in sample buffer to interpolate between
        int src_pos = int_index * 2;
        int next_src_pos = src_pos + 2;
        if (next_src_pos >= SOURCE_BUFFER_SIZE * 2) next_src_pos = src_pos;

        // calculate target samples
        target_buffer.new[2 * i] = source_sample_buffer[src_pos] * (1.0f - frac) + source_sample_buffer[next_src_pos] * frac;
        target_buffer.new[2 * i + 1] = source_sample_buffer[src_pos + 1] * (1.0f - frac) + source_sample_buffer[next_src_pos + 1] * frac;

        // increment index (location of target sample relative to source samples)
        index += step;
    }

    // Find trigger in middle half of combined buffer
    int trigger_index = find_trigger_point();

    // Shift waveform to keep trigger centered
    // This will update the audio render_buffer
    shift_waveform(trigger_index);

    // Reset source sample buffer
    source_buffer_count = 0;
}

void queue_audio() {    
    // Get size of already-queued audio samples
    u32 queued_audio_size = SDL_GetQueuedAudioSize(dev);
    // printf("size: %d\n",queued_audio_size);
 
    // If queue is too low, add some extra samples to prevent popping
    if (queued_audio_size < 2 * 2 * actual_target_frames * sizeof(float)) {
        SDL_QueueAudio(dev, target_buffer.new, actual_target_frames * 2 * sizeof(float));
    }

    // Add samples from buffer to audio queue (but not if queue is too large)
    while (SDL_GetQueuedAudioSize(dev) > 4 * 2 * actual_target_frames * sizeof(float)) {
        SDL_Delay(1);
        // printf("Delay 1 ms---------------------------------\n");
    }

    SDL_QueueAudio(dev, target_buffer.new, actual_target_frames * 2 * sizeof(float));
}

// Function to find trigger in middle half of combined buffer
int find_trigger_point() {
    float trigger_threshold = -1.0f;
    int start = TARGET_FRAMES / 2;  // Middle of prev buffer
    int end = TARGET_FRAMES + (TARGET_FRAMES / 2);  // Middle of new buffer
    for (int i = end; i > start; i--) {
        if (target_buffer.combined[i * 2] > trigger_threshold && target_buffer.combined[(i - 1) * 2] <= trigger_threshold) { // Zero-crossing (left channel)
            return i;
        }
    }
    return end; // Default: no trigger found, use end of middle section
}

// Function to shift buffer so trigger is centered
void shift_waveform(int trigger_index) {   
    int shift_amount = trigger_index - (TARGET_FRAMES / 2);
    for (int i = 0; i < TARGET_FRAMES; i++) {
        render_buffer[i * 2] = target_buffer.combined[(i + shift_amount) * 2];     // Left channel
        render_buffer[i * 2 + 1] = target_buffer.combined[(i + shift_amount) * 2 + 1]; // Right channel
    }
}