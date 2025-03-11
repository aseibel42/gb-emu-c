#include <SDL2/SDL.h>
#include <stdio.h>

#include "apu.h"
#include "io.h"

#define BUFFER_SIZE 1024
#define SAMPLE_RATE 48000 

u8 frame_sequence_counter = 0;
u8 ch2_length_counter = 0;
u8 vol_env_counter = 0;
u16 period_counter = 0;
u8 sample_tick_counter = 0;

u8 ch2_current_volume = 0;
u8 wave_duty_bit = 0;

u16 buffer_count = 0;
bool ch2_dac_enabled = true;
float sample_buffer[BUFFER_SIZE] = { 0.0F };

SDL_AudioDeviceID device;

const u8 wave_duty_table[4] = {
    0b00000001,  // 12.5% duty cycle
    0b10000001,  // 25% duty cycle
    0b10000111,  // 50% duty cycle
    0b01111110   // 75% duty cycle
};

void apu_init() {
    // setup sdl for audio
    SDL_setenv("SDL_AUDIODRIVER", "directsound", 1); // not sure if this is necessary?
    SDL_Init(SDL_INIT_AUDIO);

    // Audio specifications
    SDL_AudioSpec want, have;
    SDL_memset(&want, 0, sizeof(want));
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_F32; // amplitude between -1 and 1
    want.channels = 2; // stereo
    want.samples = BUFFER_SIZE;
    want.callback = NULL;  // Using SDL_QueueAudio()

    // Open audio device
    device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (device == 0) {
        SDL_Log("Failed to open audio: %s\n", SDL_GetError());
    }
    else {
        printf("SDL Init Audio\n");
    }

    // open audio device, set sample rate and buffer size
    // SDLInitAudio(SAMPLE_RATE, BUFFER_SIZE);

    // unpause SDL audio
    SDL_PauseAudioDevice(device, 0);
}

void apu_tick() {
    // Increment period counter, if 2048, it gets reset to the period in NR23/24 and wavedutybit is incremented
    period_counter++;
    if(period_counter >= 2048) {
        apu_set_period(); // ghostboy says this should be (2048-period)*4
        wave_duty_bit = (wave_duty_bit + 1) & 0b111;
    }

    // Get a new sample every so many ticks (4194304 Hz clock / 48000 Hz sampling rate = 87.4, so 1 sample roughly every 87 ticks)
    sample_tick_counter++;
    if(sample_tick_counter == 87) {
        ch2_step(); // get new audio sample for ch2
        sample_tick_counter = 0; // reset counter
    }
}

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

// Every 256 Hz - 16384 ticks - 4096 M-cycles
void length_counter_tick() {
    if (ch2_length_counter < 64 && io.ch2_ctrl.length_enable) {
        ch2_length_counter++;
        if (ch2_length_counter > 63) {
            io.master_ctrl.ch2_enable = false; // turn ch2 off
        }
    } 
}

// Every 64 Hz - 16384 M-cycles
void volume_envelope_tick() {
    // do nothing if pace = 0
    if(io.ch2_vol.pace) {
        vol_env_counter++;
        // add or subtract 1 from volume depending on whether direction bit is set
        // ^ only do this once every sweep pace (1-7)
        if(vol_env_counter == io.ch2_vol.pace) {
            if (io.ch2_vol.dir) {
                if (ch2_current_volume < 15) {
                    ch2_current_volume++;
                }
            } else {
                if (ch2_current_volume > 0) {
                    ch2_current_volume--;
                }
            }
            vol_env_counter = 0;
        }
    }
}

void sweep_tick() {

}

void ch2_step() {
    // Retrieve wave pattern from highest 2 bits of NR21
    u8 wave_pattern = (io.ch2_len >> 6) & 0b11;

    // Rescale output volume (0 -> -1, 15 -> 1)
    float output_volume = (ch2_current_volume / 7.5f) - 1.0f;
    
    // if channel enabled, dac enabled, and waveduty bit is 1, set outputVol to curVol, otherwise     set to 0
    sample_buffer[buffer_count++] = (io.master_pan.ch2_left && ch2_dac_enabled && apu_get_wave_bit(wave_pattern, wave_duty_bit)) ? output_volume : 0; 
    sample_buffer[buffer_count++] = (io.master_pan.ch2_right && ch2_dac_enabled && apu_get_wave_bit(wave_pattern, wave_duty_bit)) ? output_volume : 0; 
    if(buffer_count >= BUFFER_SIZE) { // use 4096 for sample_size? Adjust this as necessary?
        printf("Buffer_count: %d\n",buffer_count);
        printf("Audio size: %d\n", SDL_GetQueuedAudioSize(device));
        printf("Sample buffer of 1023: %f\n",sample_buffer[1023]);
        // send sample to speakers until buffer almost empty
        while ((SDL_GetQueuedAudioSize(device)) > BUFFER_SIZE * sizeof(float)) {
	        SDL_Delay(1);
        }
        SDL_QueueAudio(device, sample_buffer, BUFFER_SIZE * sizeof(float));
        // printf("Queueing audio\n");

//         // reset buffer count
        buffer_count = 0;
    }
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
    vol_env_counter = 0;

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