#include <SDL2/SDL.h>
#include <stdio.h>

#include "apu.h"

// External
extern int cpu_speed;

// General counters
u8 frame_sequence_counter = 0;
u8 sample_tick_counter = 0;
u8 apu_speed_counter = 0;

// Audio channels
SquareChannel ch1 = {0};
SquareChannel ch2 = {0};
WaveChannel ch3 = {0};

// Audio buffer
u16 source_buffer_count = 0;
float* combined_target_buffer = NULL;

// Audio device
SDL_AudioSpec desired, obtained;
SDL_AudioDeviceID dev;

const u8 ch3_vol_shift_map[4] = {
    4, // 0% vol
    0, // 100% vol
    1, // 50% vol
    2  // 25% vol
};

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

    // Initialize square channels
    ch1 = init_square_channel(1);
    ch2 = init_square_channel(2);
    ch3 = init_wave_channel();

    // Initialize and zero out the combined target buffer
    combined_target_buffer = malloc(TARGET_FRAMES * 2 * sizeof(float));
    memset(combined_target_buffer, 0, TARGET_FRAMES * 2 * sizeof(float));

    // Unpause audio device
    SDL_PauseAudioDevice(dev, 0);
}

// Function to initialize a SquareChannel struct
SquareChannel init_square_channel(u8 ch_num) {
    SquareChannel ch = {0}; // Zero-initialize the struct

    // Set default values
    ch.period_counter = 0;
    ch.length_counter = 0;
    ch.vol_env_counter = 0;
    ch.wave_duty_bit_counter = 0;

    ch.current_vol = 0;
    ch.dac_enable = true;
    ch.vol_env_enable = false;

    ch.trigger_index = TARGET_FRAMES + (TARGET_FRAMES / 2);

    // Allocate buffers on the heap
    ch.source_sample_buffer = malloc(SOURCE_BUFFER_SIZE * 2 * sizeof(float));
    ch.target_sample_buffer = malloc(sizeof(AudioBuffer));

    // Check for allocation failure
    if (!ch.source_sample_buffer || !ch.target_sample_buffer) {
        printf("Error: Failed to allocate memory for buffers\n");
    }

    // Initialize target buffer
    memset(ch.target_sample_buffer, -1, sizeof(AudioBuffer));

    // Channel-specific references
    switch (ch_num) {
        case 1:
            ch.master_ctrl_bit = 0;
            ch.master_vol_left_bit = 4;
            ch.master_vol_right_bit = 0;
            ch.ch_length = &io.ch1_len;
            ch.ch_vol = &io.ch1_vol;
            ch.ch_freq = &io.ch1_freq;
            ch.ch_ctrl = &io.ch1_ctrl;
            break;
        case 2:
            ch.master_ctrl_bit = 1;
            ch.master_vol_left_bit = 5;
            ch.master_vol_right_bit = 1;
            ch.ch_length = &io.ch2_len;
            ch.ch_vol = &io.ch2_vol;
            ch.ch_freq = &io.ch2_freq;
            ch.ch_ctrl = &io.ch2_ctrl;
            break;
    }

    return ch;  // Return the initialized struct
}

// Function to initialize a WaveChannel struct
WaveChannel init_wave_channel() {
    WaveChannel ch = {0}; // Zero-initialize the struct

    // Set default values
    ch.period_counter = 0;
    ch.length_counter = 0;
    ch.wave_duty_bit_counter = 0;

    ch.dac_enable = false;

    ch.trigger_index = TARGET_FRAMES + (TARGET_FRAMES / 2);

    // Allocate buffers on the heap
    ch.source_sample_buffer = malloc(SOURCE_BUFFER_SIZE * 2 * sizeof(float));
    ch.target_sample_buffer = malloc(sizeof(AudioBuffer));

    // Check for allocation failure
    if (!ch.source_sample_buffer || !ch.target_sample_buffer) {
        printf("Error: Failed to allocate memory for buffers\n");
    }

    // Initialize target buffer
    memset(ch.target_sample_buffer, -1, sizeof(AudioBuffer));

    return ch;  // Return the initialized struct
}

// APU ticks once every M-cycle
void apu_tick() {
    // Increment period counter
    apu_speed_counter++; // apu_speed_counter rescales audio freq at higher cpu_speed back to normal
    if (apu_speed_counter == cpu_speed) {
        period_counter_tick();
        apu_speed_counter = 0;
    }

    // Get a new sample every 4 M-ticks - 17556 M-ticks per frame at 60 FPS -> (17,556 / 32) * 8 => 4389 audio samples / frame for 256kHz sample rate
    sample_tick_counter++;
    if(sample_tick_counter == (4 * cpu_speed)) {
        if (source_buffer_count < SOURCE_BUFFER_SIZE) {
            generate_source_audio_samples(&ch1);
            generate_source_audio_samples(&ch2);
            ch3_generate_source_audio_samples();
            source_buffer_count++;
        }
        sample_tick_counter = 0; // reset counter
    }
}

// Increment period counter, if 2048, it gets reset to the period in NR23/24 and wavedutybit is incremented
void period_counter_tick() {
    square_period_counter_tick(&ch1);
    square_period_counter_tick(&ch2);
    wave_period_counter_tick();
    wave_period_counter_tick(); // wave period counter happens 2x the freq of other period counters
}

void square_period_counter_tick(SquareChannel *ch) {
    ch->period_counter++;
    if(ch->period_counter >= 2048) {
        u16 period = ((u16)ch->ch_ctrl->period << 8) | *(ch->ch_freq);
        ch->period_counter = period;
        ch->wave_duty_bit_counter = (ch->wave_duty_bit_counter + 1) & 0b111; // 0-7
    }
}

void wave_period_counter_tick() {
    ch3.period_counter++;
    if(ch3.period_counter >= 2048) {
        u16 period = ((u16)io.ch3_ctrl.period << 8) | io.ch3_freq;
        ch3.period_counter = period;
        ch3.wave_duty_bit_counter = (ch3.wave_duty_bit_counter + 1) & 0b11111; // 0-31
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
    square_length_counter_tick(&ch1);
    square_length_counter_tick(&ch2);
    wave_length_counter_tick();
}

void square_length_counter_tick(SquareChannel *ch) {
    // Length counter ticks up to 64.  If it reaches 64, turn channel off
    if (ch->length_counter < 64 && ch->ch_ctrl->length_enable) {
        ch->length_counter++;
        if (ch->length_counter > 63) {
            io.master_ctrl.value &= ~(1 << ch->master_ctrl_bit); // turn ch off by setting appropriate master ctrl bit to 0
        }
    }
}

void wave_length_counter_tick() {
    // Length counter ticks up to 256.  If it reaches 256, turn channel off
    if (ch3.length_counter < 256 && io.ch3_ctrl.length_enable) {
        ch3.length_counter++;
        if (ch3.length_counter > 256) {
            io.master_ctrl.ch3_enable = 0; // turn ch 3 off by setting appropriate master ctrl bit to 0
        }
    }
}

// Call at 64 Hz - once every 16384 M-cycles
void volume_envelope_tick() {
    square_vol_env_tick(&ch1);
    square_vol_env_tick(&ch2);
}

void square_vol_env_tick(SquareChannel *ch) {
    // env should be disabled if pace = 0
    if(ch->vol_env_enable && (io.master_ctrl.value >> ch->master_ctrl_bit) & 1) {
        ch->vol_env_counter++;

        // Every time that vol_evn pace (1-7) is reached, change volume
        // add 1 to volume if direction bit set, sub 1 if not set
        if(ch->vol_env_counter == ch->ch_vol->pace) {
            // Reset envelope counter once pace is reached
            ch->vol_env_counter = 0;

            // If direction bit is set then crescendo, otherwise decrescendo.  Lock vol between 0 and 15
            if (ch->ch_vol->dir) {
                if (ch->current_vol < 15) {
                    ch->current_vol++;
                }
            } else {
                if (ch->current_vol > 0) {
                    ch->current_vol--;
                }
            }

            // Disable vol_env if volume reaches 0 or 15
            if (ch->current_vol == 0 || ch->current_vol == 15) {
                ch->vol_env_enable = false;
            }
        }
    }
}

void sweep_tick() {

}

void square_trigger(SquareChannel *ch) {
    // enable ch on master_ctrl register
    io.master_ctrl.value |= (1 << ch->master_ctrl_bit);

    // if length timer is expired, reset it to initial value stored in NRX1
    if (ch->length_counter >= 64) {
        ch->length_counter = *(ch->ch_length) & 0b111111;
    }

    // reset period counter (set to initial value in NRX3/X4)
    u16 period = ((u16)ch->ch_ctrl->period << 8) | *(ch->ch_freq);
    ch->period_counter = period;

    // reset volume envelope timer
    ch->vol_env_counter = 0;

    // reset current volume to initial volume in NRX2
    ch->current_vol = ch->ch_vol->init_vol;

}

void ch3_trigger() {
    // enable ch on master_ctrl register
    io.master_ctrl.ch3_enable = true;

    // if length timer is expired, reset it to initial value stored in NRX1
    if (ch3.length_counter >= 256) {
        ch3.length_counter = io.ch3_len;
    }

    // reset period counter (set to initial value in NRX3/X4)
    u16 period = ((u16)io.ch3_ctrl.period << 8) | io.ch3_freq;
    ch3.period_counter = period;

    // Reset wave position
    ch3.wave_duty_bit_counter = 0;
}

void square_handle_volume_write(SquareChannel *ch) {
    // dac gets disabled if upper 5 bits of NRX2 are all 0
    ch->dac_enable = ((ch->ch_vol->value & 0xF8) != 0);

    // enable ch vol envelope if pace > 0
    ch->vol_env_enable = ch->ch_vol->pace;

    // set ch vol envelope counter to 0
    ch->vol_env_counter = 0;

    // set ch current_volume to this vol register's init vol
    ch->current_vol = ch->ch_vol->init_vol;
}

void generate_source_audio_samples(SquareChannel *ch) {
    // Retrieve wave pattern from highest 2 bits of NRX1
    // look up wave bit (0 or 1) knowing wave pattern and wave duty bit counter
    u8 ch_wave_pattern = (*(ch->ch_length) >> 6) & 0b11;
    u8 wave_bit = (wave_duty_table[ch_wave_pattern] >> (7 - ch->wave_duty_bit_counter)) & 1;

    // Rescale output volume (0 -> -1, 15 -> 1)
    float ch_output_volume = ((float)(ch->current_vol) / 7.5f) - 1.0f;

    // scale volume based on master left and right volume (value of 0-7 refers to vol level of 1-8, divide by 8 to rescale between -1 and 1)
    float ch_left_output_volume = (ch_output_volume + 1) * (io.master_vol.vol_left + 1)/8 - 1;
    float ch_right_output_volume = (ch_output_volume + 1) * (io.master_vol.vol_right + 1)/8 - 1;

    // left and right vol pan (true or false)
    bool left_pan = (io.master_pan.value >> ch->master_vol_left_bit) & 1;
    bool right_pan = (io.master_pan.value >> ch->master_vol_right_bit) & 1;

    // if channel enabled, dac enabled, and waveduty bit is 1, set sample value to calc. output vol, otherwise set to -1 (first L, then R)
    ch->source_sample_buffer[2*source_buffer_count] = (left_pan && ch->dac_enable && wave_bit) ? ch_left_output_volume : -1;
    ch->source_sample_buffer[2*source_buffer_count+1] = (right_pan && ch->dac_enable && wave_bit) ? ch_right_output_volume : -1;
}

void ch3_generate_source_audio_samples() {
    // Retrieve wave pattern from highest 2 bits of NRX1
    // look up wave bit (0 or 1) knowing wave pattern and wave duty bit counter
    u8 output_level = (io.ch3_vol >> 5) & 0b11;  // 0b00, 0b01, 0b10, or 0b11
    u8 shift_amount = ch3_vol_shift_map[output_level]; // 0, 100, 50, or 25% vol
    u8 current_vol = (ch3_get_wave_nibble(ch3.wave_duty_bit_counter)) >> shift_amount; // 1 or 0

    // Rescale output volume (0 -> -1, 15 -> 1)
    float ch_output_volume = ((float)current_vol / 7.5f) - 1.0f;

    // scale volume based on master left and right volume (value of 0-7 refers to vol level of 1-8, divide by 8 to rescale between -1 and 1)
    float ch_left_output_volume = (ch_output_volume + 1) * (io.master_vol.vol_left + 1)/8 - 1;
    float ch_right_output_volume = (ch_output_volume + 1) * (io.master_vol.vol_right + 1)/8 - 1;

    // left and right vol pan (true or false)
    bool left_pan = (io.master_pan.value >> io.master_vol.vol_left) & 1;
    bool right_pan = (io.master_pan.value >> io.master_vol.vol_right) & 1;

    // if channel enabled, dac enabled, and waveduty bit is 1, set sample value to calc. output vol, otherwise set to -1 (first L, then R)
    ch3.source_sample_buffer[2*source_buffer_count] = (left_pan && ch3.dac_enable) ? ch_left_output_volume : -1;
    ch3.source_sample_buffer[2*source_buffer_count+1] = (right_pan && ch3.dac_enable) ? ch_right_output_volume : -1;
}

u8 ch3_get_wave_nibble(u8 index) {
    // Ensure the index is within the valid range of 0 to 31
    if (index > 31) {
        printf("Error: invalid wave index");
        return 0;
    }

    // Find which byte and nibble within that byte to extract
    u8 byte_index = index / 2;    // Determine the byte index (0 to 15)
    u8 nibble_index = index % 2;  // Determine whether it's the high or low nibble (0 or 1)

    // Get the byte from the array
    u8 byte = io.wave[byte_index];

    // Extract the correct nibble (high or low) using bitwise operations
    if (nibble_index == 0) {
        return (byte >> 4) & 0x0F;  // Extract the high nibble
    } else {
        return byte & 0x0F;         // Extract the low nibble
    }
}

// resample audio from source sample rate (262144 Hz) to sample rate of audio device (48000 Hz)
void resample_audio() {
    // Save new buffer as previous buffer for next iteration
    memcpy(ch1.target_sample_buffer->prev, ch1.target_sample_buffer->curr, sizeof(float) * TARGET_FRAMES * 2);
    memcpy(ch2.target_sample_buffer->prev, ch2.target_sample_buffer->curr, sizeof(float) * TARGET_FRAMES * 2);
    memcpy(ch3.target_sample_buffer->prev, ch3.target_sample_buffer->curr, sizeof(float) * TARGET_FRAMES * 2);
    // printf("source_buffer_count: %d\n",source_buffer_count);

    // find number of source frames per target frame - (262144/48000 ~ 5.46)
    float step = (float)SOURCE_SAMPLE_RATE / TARGET_SAMPLE_RATE;

    // calculate target audio samples by linearly interpolating between source audio samples
    float index = 0.0f;
    for (u32 i = 0; i < TARGET_FRAMES; i++) {
        // find relative distance of target sample from first source sample
        int int_index = (int)index;
        float frac = index - int_index;

        // find points in sample buffer to interpolate between
        int src_pos = int_index * 2;
        int next_src_pos = src_pos + 2;
        if (next_src_pos >= SOURCE_BUFFER_SIZE * 2) next_src_pos = src_pos;

        // calculate target samples
        ch1.target_sample_buffer->curr[2 * i] = ch1.source_sample_buffer[src_pos] * (1.0f - frac) + ch1.source_sample_buffer[next_src_pos] * frac;
        ch1.target_sample_buffer->curr[2 * i + 1] = ch1.source_sample_buffer[src_pos + 1] * (1.0f - frac) + ch1.source_sample_buffer[next_src_pos + 1] * frac;

        ch2.target_sample_buffer->curr[2 * i] = ch2.source_sample_buffer[src_pos] * (1.0f - frac) + ch2.source_sample_buffer[next_src_pos] * frac;
        ch2.target_sample_buffer->curr[2 * i + 1] = ch2.source_sample_buffer[src_pos + 1] * (1.0f - frac) + ch2.source_sample_buffer[next_src_pos + 1] * frac;

        ch3.target_sample_buffer->curr[2 * i] = ch3.source_sample_buffer[src_pos] * (1.0f - frac) + ch3.source_sample_buffer[next_src_pos] * frac;
        ch3.target_sample_buffer->curr[2 * i + 1] = ch3.source_sample_buffer[src_pos + 1] * (1.0f - frac) + ch3.source_sample_buffer[next_src_pos + 1] * frac;

        // increment index (location of target sample relative to source samples)
        index += step;
    }

    // Find trigger in middle half of combined buffer
    ch1.trigger_index = find_trigger_point(ch1.target_sample_buffer->combined);
    ch2.trigger_index = find_trigger_point(ch2.target_sample_buffer->combined);
    ch3.trigger_index = ch3_find_trigger_point(ch3.target_sample_buffer->combined);

    // Reset source sample buffer
    source_buffer_count = 0;
}

void queue_audio() {
    // mix audio
    mix_buffers(ch1.target_sample_buffer->curr, ch2.target_sample_buffer->curr, ch3.target_sample_buffer->curr, combined_target_buffer);

    // Add samples from buffer to audio queue (but not if queue is too large)
    while (SDL_GetQueuedAudioSize(dev) > 4 * TARGET_FRAMES * 2 * sizeof(float)) {
        SDL_Delay(1);
        // printf("Delay 1 ms---------------------------------\n");
    }

    SDL_QueueAudio(dev, combined_target_buffer, TARGET_FRAMES * 2 * sizeof(float));
}

// Function to find trigger in middle half of combined buffer
u16 find_trigger_point(float buffer[]) {
    float trigger_threshold = -1.0f;
    int start = TARGET_FRAMES / 2;  // Middle of prev buffer
    int end = TARGET_FRAMES + (TARGET_FRAMES / 2);  // Middle of new buffer
    for (int i = end; i > start; i--) {
        if (buffer[i * 2] > trigger_threshold && buffer[(i - 1) * 2] <= trigger_threshold) { // Zero-crossing (left channel)
            return i;
        }
    }
    return end; // Default: no trigger found, use end of middle section
}

// Function to find trigger in ch3 (find beginning of wave pattern)
u16 ch3_find_trigger_point(float buffer[]) {
    u8 output_level = (io.ch3_vol >> 5) & 0b11;  // 0b00, 0b01, 0b10, or 0b11
    u8 shift_amount = ch3_vol_shift_map[output_level]; // 0, 100, 50, or 25% vol
    u8 wave_bit_vol = (ch3_get_wave_nibble(0)) >> shift_amount; // 1 or 0
    float wave_bit_zero_output_vol = ((float)wave_bit_vol / 7.5f) - 1.0f;
    wave_bit_zero_output_vol = (wave_bit_zero_output_vol + 1) * (io.master_vol.vol_left + 1)/8 - 1;

    wave_bit_vol = (ch3_get_wave_nibble(1)) >> shift_amount; // 1 or 0
    float wave_bit_one_output_vol = ((float)wave_bit_vol / 7.5f) - 1.0f;
    wave_bit_one_output_vol = (wave_bit_one_output_vol + 1) * (io.master_vol.vol_left + 1)/8 - 1;

    int start = TARGET_FRAMES / 2;  // Middle of prev buffer
    int end = TARGET_FRAMES + (TARGET_FRAMES / 2);  // Middle of new buffer
    // find trigger point
    for (int i = end; i > start; i--) {
        if (buffer[i * 2] == wave_bit_one_output_vol && buffer[(i - 1) * 2] == wave_bit_zero_output_vol) { // Zero-crossing (left channel)
            return i - 1;
        }
    }

    return ch3_find_trigger_min(ch3.target_sample_buffer->combined); // If wave beginning not found, try different triggering method
}

// Function to find trigger in ch3 (find min, then first trigger from right)
u16 ch3_find_trigger_min(float buffer[]) {
    float trigger_threshold = 1.0f; // Initialize to the largest possible float value
    int start = TARGET_FRAMES / 2;  // Middle of prev buffer
    int end = TARGET_FRAMES + (TARGET_FRAMES / 2);  // Middle of new buffer
    // find trigger_threshold
    for (int i = end; i > start; i--) {
        if (buffer[i] < trigger_threshold) {
            trigger_threshold = buffer[i];
        }
    }
    // find trigger point
    for (int i = end; i > start; i--) {
        if (buffer[i * 2] > trigger_threshold && buffer[(i - 1) * 2] <= trigger_threshold) { // Zero-crossing (left channel)
            return i;
        }
    }
    return end; // Default: no trigger found, use end of middle section
}

// Function to mix audio buffers (average them)
void mix_buffers(const float *ch1, const float *ch2, const float *ch3, float *result) {
    for (int i = 0; i < TARGET_FRAMES * 2; i++) {
        result[i] = (ch1[i] + ch2[i] + ch3[i]) / 3.0f;
    }
}
