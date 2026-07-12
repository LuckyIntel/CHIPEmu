/*
    A simple High-Level Audio wrapper for
    miniaudio.h
*/
#ifndef AUDIO_H
#define AUDIO_H
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>
#include <string.h>
#include "CPU.h"

ma_device MINIAUDIO_DEVICE;
ma_waveform MINIAUDIO_WAVEFORM;

/*
    Not something we need to call ourself.
    MiniAudio handles the calling of this function.
*/
void AUDIO_EVENT(ma_device* device, void* output, const void* input, ma_uint32 frame)
{
    if (CHIP8.soundTimer > 0) ma_waveform_read_pcm_frames(&MINIAUDIO_WAVEFORM, output, frame, NULL);
    else memset(output, 0, frame * ma_get_bytes_per_frame(device->playback.format, device->playback.channels));
};

/*
    Sets up the audio for CHIP-8 sounds.

    Audio is MONO.
    Starts a process that runs in the background.
    We do not need to check ourself if we need to
    play a sound. Just initialize this function.
    
    Returns 0 if fails to initialize.
    Returns 1 if completes without any problem.
*/
int initAudio()
{
    ma_device_config settings;
    ma_waveform_config waveConf;

    waveConf = ma_waveform_config_init( ma_format_f32, 1, 44100, ma_waveform_type_sine, 440.0, 0.2);

    if (ma_waveform_init(&waveConf, &MINIAUDIO_WAVEFORM) != MA_SUCCESS) return 0;

    settings = ma_device_config_init(ma_device_type_playback);
    settings.playback.format = ma_format_f32;
    settings.playback.channels = 1;
    settings.sampleRate = 44100;
    settings.dataCallback = AUDIO_EVENT;

    if (ma_device_init(NULL, &settings, &MINIAUDIO_DEVICE) != MA_SUCCESS) return 0;
    if (ma_device_start(&MINIAUDIO_DEVICE)) { ma_device_uninit(&MINIAUDIO_DEVICE); ma_waveform_uninit(&MINIAUDIO_WAVEFORM); return 0; };

    return 1;
};

/*
    Terminates MINIAUDIO_DEVICE and MINIAUDIO_WAVEFORM
*/
void terminateAudio()
{
    ma_device_uninit(&MINIAUDIO_DEVICE);
    ma_waveform_uninit(&MINIAUDIO_WAVEFORM);
};

#endif