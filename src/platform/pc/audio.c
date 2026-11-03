// Sound output: the m4a mixer's Direct Sound samples mixed with an emulation
// of the four Game Boy (CGB) sound channels, played with SDL3.
//
// The m4a engine writes the CGB registers once per frame (in SoundMain) and then
// hands over that frame's Direct Sound samples. The CGB channels are run for the
// same number of samples, reading the registers from I/O memory. Writes that
// restart a channel are seen as bit 7 of NRx4, which is cleared after handling.

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"

#define NR(x) (*(volatile uint8_t*)(PC_IO + (x)))
#define NR10 NR(0x60)
#define NR11 NR(0x62)
#define NR12 NR(0x63)
#define NR13 NR(0x64)
#define NR14 NR(0x65)
#define NR21 NR(0x68)
#define NR22 NR(0x69)
#define NR23 NR(0x6C)
#define NR24 NR(0x6D)
#define NR30 NR(0x70)
#define NR31 NR(0x72)
#define NR32 NR(0x73)
#define NR33 NR(0x74)
#define NR34 NR(0x75)
#define NR41 NR(0x78)
#define NR42 NR(0x79)
#define NR43 NR(0x7C)
#define NR44 NR(0x7D)
#define NR50 NR(0x80)
#define NR51 NR(0x81)
#define SOUNDCNT_H PC_REG16(0x82)
#define NR52 NR(0x84)
#define WAVE_RAM (PC_IO + 0x90)

int gPcAudioEnabled;

// TMC_WAV=path records the output (also when headless), for testing.
static FILE* sWav;
static uint32_t sWavSamples;
static int sWavFrequency;

static void WriteWavHeader(void) {
    uint32_t dataSize = sWavSamples * 2;
    uint8_t h[44];
    memcpy(h, "RIFF", 4);
    *(uint32_t*)(h + 4) = 36 + dataSize;
    memcpy(h + 8, "WAVEfmt ", 8);
    *(uint32_t*)(h + 16) = 16;
    *(uint16_t*)(h + 20) = 1; // PCM
    *(uint16_t*)(h + 22) = 1; // mono
    *(uint32_t*)(h + 24) = sWavFrequency;
    *(uint32_t*)(h + 28) = sWavFrequency * 2;
    *(uint16_t*)(h + 32) = 2;
    *(uint16_t*)(h + 34) = 16;
    memcpy(h + 36, "data", 4);
    *(uint32_t*)(h + 40) = dataSize;
    fseek(sWav, 0, SEEK_SET);
    fwrite(h, 1, sizeof(h), sWav);
    fseek(sWav, 0, SEEK_END);
}

static void CloseWav(void) {
    WriteWavHeader();
    fclose(sWav);
}

static void RecordWav(const int16_t* samples, int count, int frequency) {
    static int checked;
    if (!checked) {
        const char* path = getenv("TMC_WAV");
        checked = 1;
        if (path != NULL && (sWav = fopen(path, "wb")) != NULL) {
            sWavFrequency = frequency;
            WriteWavHeader();
            atexit(CloseWav);
        }
    }
    if (sWav != NULL && frequency == sWavFrequency) {
        fwrite(samples, 2, count, sWav);
        sWavSamples += count;
    }
}

typedef struct {
    int on;
    int length;
    int volume;
    int envTimer;
    uint32_t phase; // 32-bit fraction of a waveform cycle
    // channel 1 sweep
    int sweepTimer;
    int sweepFreq;
    // noise
    uint16_t lfsr;
    double noiseClock;
} PsgChannel;

static PsgChannel sChannels[4];
static double sSequencer; // 512 Hz frame sequencer
static int sSequencerStep;
static SDL_AudioStream* sStream;
static int sStreamFrequency;

static const uint8_t sDuty[4] = { 0x01, 0x81, 0x87, 0x7E };

static void StartEnvelope(PsgChannel* ch, uint8_t nrx2) {
    ch->volume = nrx2 >> 4;
    ch->envTimer = nrx2 & 7;
    if ((nrx2 & 0xF8) == 0) // DAC off
        ch->on = 0;
}

static void ClockEnvelope(PsgChannel* ch, uint8_t nrx2) {
    int period = nrx2 & 7;
    if (period == 0 || --ch->envTimer > 0)
        return;
    ch->envTimer = period;
    if ((nrx2 & 8) && ch->volume < 15)
        ch->volume++;
    else if (!(nrx2 & 8) && ch->volume > 0)
        ch->volume--;
}

static void ClockLength(PsgChannel* ch, uint8_t nrx4) {
    if ((nrx4 & 0x40) && ch->length > 0 && --ch->length == 0)
        ch->on = 0;
}

static int SweepCalc(PsgChannel* ch) {
    int delta = ch->sweepFreq >> (NR10 & 7);
    int freq = (NR10 & 8) ? ch->sweepFreq - delta : ch->sweepFreq + delta;
    if (freq > 2047)
        ch->on = 0;
    return freq;
}

static void ClockSweep(PsgChannel* ch) {
    int period = (NR10 >> 4) & 7;
    if (period == 0 || --ch->sweepTimer > 0)
        return;
    ch->sweepTimer = period;
    if (NR10 & 7) {
        int freq = SweepCalc(ch);
        if (ch->on && freq >= 0) {
            ch->sweepFreq = freq;
            NR13 = freq & 0xFF;
            NR14 = (NR14 & ~7) | ((freq >> 8) & 7);
            SweepCalc(ch);
        }
    }
}

static void HandleTriggers(void) {
    PsgChannel* ch;

    if (NR14 & 0x80) {
        ch = &sChannels[0];
        NR14 &= 0x7F;
        ch->on = 1;
        if (ch->length == 0)
            ch->length = 64 - (NR11 & 63);
        ch->sweepFreq = NR13 | ((NR14 & 7) << 8);
        ch->sweepTimer = (NR10 >> 4) & 7;
        StartEnvelope(ch, NR12);
        if (NR10 & 7)
            SweepCalc(ch);
    }
    if (NR24 & 0x80) {
        ch = &sChannels[1];
        NR24 &= 0x7F;
        ch->on = 1;
        if (ch->length == 0)
            ch->length = 64 - (NR21 & 63);
        StartEnvelope(ch, NR22);
    }
    if (NR34 & 0x80) {
        ch = &sChannels[2];
        NR34 &= 0x7F;
        ch->on = (NR30 & 0x80) != 0;
        if (ch->length == 0)
            ch->length = 256 - NR31;
        ch->phase = 0;
    }
    if (NR44 & 0x80) {
        ch = &sChannels[3];
        NR44 &= 0x7F;
        ch->on = 1;
        if (ch->length == 0)
            ch->length = 64 - (NR41 & 63);
        ch->lfsr = 0x7FFF;
        StartEnvelope(ch, NR42);
    }
    if (!(NR30 & 0x80))
        sChannels[2].on = 0;
}

static void ClockSequencer(void) {
    int step = sSequencerStep++ & 7;
    if (!(step & 1)) { // 256 Hz
        ClockLength(&sChannels[0], NR14);
        ClockLength(&sChannels[1], NR24);
        ClockLength(&sChannels[2], NR34);
        ClockLength(&sChannels[3], NR44);
    }
    if (step == 2 || step == 6) // 128 Hz
        ClockSweep(&sChannels[0]);
    if (step == 7) { // 64 Hz
        ClockEnvelope(&sChannels[0], NR12);
        ClockEnvelope(&sChannels[1], NR22);
        ClockEnvelope(&sChannels[3], NR42);
    }
}

static int SquareSample(PsgChannel* ch, uint8_t nrx1, int freq, double rate) {
    uint32_t step = (uint32_t)(131072.0 / (2048 - freq) / rate * 4294967296.0);
    int bit = (sDuty[nrx1 >> 6] >> (ch->phase >> 29)) & 1;
    ch->phase += step;
    return bit ? ch->volume : -ch->volume;
}

static int WaveSample(PsgChannel* ch, double rate) {
    int freq = NR33 | ((NR34 & 7) << 8);
    uint32_t step = (uint32_t)(65536.0 / (2048 - freq) / rate * 4294967296.0);
    int pos = ch->phase >> 27; // 32 samples
    int v = ((WAVE_RAM[pos >> 1] >> ((pos & 1) ? 0 : 4)) & 0xF) - 8;
    ch->phase += step;
    if (NR32 & 0x80) // 75%
        return (v * 3 / 4) * 2;
    switch ((NR32 >> 5) & 3) {
        case 1:
            return v * 2;
        case 2:
            return (v >> 1) * 2;
        case 3:
            return (v >> 2) * 2;
    }
    return 0;
}

static int NoiseSample(PsgChannel* ch, double rate) {
    int ratio = NR43 & 7, shift = NR43 >> 4;
    double freq = 524288.0 / (ratio ? ratio : 0.5) / (double)(2 << shift);
    ch->noiseClock += freq / rate;
    while (ch->noiseClock >= 1.0) {
        int bit = (ch->lfsr ^ (ch->lfsr >> 1)) & 1;
        ch->noiseClock -= 1.0;
        ch->lfsr = (ch->lfsr >> 1) | (bit << 14);
        if (NR43 & 8)
            ch->lfsr = (ch->lfsr & ~0x40) | (bit << 6);
    }
    return (ch->lfsr & 1) ? -ch->volume : ch->volume;
}

static int OpenStream(int frequency) {
    SDL_AudioSpec spec = { SDL_AUDIO_S16, 1, frequency };

    if (sStream == NULL) {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
            SDL_Log("tmc: no audio: %s", SDL_GetError());
            gPcAudioEnabled = 0;
            return 0;
        }
        sStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
        if (sStream == NULL) {
            SDL_Log("tmc: no audio: %s", SDL_GetError());
            gPcAudioEnabled = 0;
            return 0;
        }
        PcApplyVolume();
        SDL_ResumeAudioStreamDevice(sStream);
        SDL_Log("tmc: audio driver %s, %d Hz", SDL_GetCurrentAudioDriver(), frequency);
    } else {
        SDL_SetAudioStreamFormat(sStream, &spec, NULL);
    }
    sStreamFrequency = frequency;
    return 1;
}

void PcApplyVolume(void) {
    if (sStream != NULL)
        SDL_SetAudioStreamGain(sStream, (float)gPcOptions.volume / PC_MAX_VOLUME);
}

// Called by SoundMain with one frame of Direct Sound samples.
void PcAudioSubmit(const int8_t* directSound, int count, int frequency) {
    static int16_t out[2048];
    uint16_t soundcntH = SOUNDCNT_H;
    int dsShift = (soundcntH & 4) ? 2 : 1;
    int dsEnabled = (soundcntH & 0x300) != 0;
    int psgShift = (soundcntH & 3) < 3 ? 2 - (soundcntH & 3) : 0; // 25%, 50%, 100%
    int masterOn = NR52 & 0x80;
    double rate = frequency;
    int i, c;

    if (frequency <= 0 || count > (int)(sizeof(out) / sizeof(out[0])))
        return;

    HandleTriggers();
    for (i = 0; i < count; i++) {
        int psg = 0, sample;

        sSequencer += 512.0 / rate;
        while (sSequencer >= 1.0) {
            sSequencer -= 1.0;
            ClockSequencer();
        }
        if (masterOn) {
            int levels[4];
            levels[0] = sChannels[0].on ? SquareSample(&sChannels[0], NR11, NR13 | ((NR14 & 7) << 8), rate) : 0;
            levels[1] = sChannels[1].on ? SquareSample(&sChannels[1], NR21, NR23 | ((NR24 & 7) << 8), rate) : 0;
            levels[2] = sChannels[2].on ? WaveSample(&sChannels[2], rate) : 0;
            levels[3] = sChannels[3].on ? NoiseSample(&sChannels[3], rate) : 0;
            for (c = 0; c < 4; c++) {
                // Mono: average of the right and left outputs.
                int right = (NR51 >> c) & 1 ? (NR50 & 7) + 1 : 0;
                int left = (NR51 >> (c + 4)) & 1 ? ((NR50 >> 4) & 7) + 1 : 0;
                psg += levels[c] * (right + left) / 2;
            }
        }
        sample = (dsEnabled ? directSound[i] << dsShift : 0) + (psg >> psgShift);
        sample *= 32;
        out[i] = sample > 32767 ? 32767 : sample < -32768 ? -32768 : sample;
    }

    // Channel status bits in NR52.
    NR52 = (NR52 & 0x80) | (sChannels[0].on ? 1 : 0) | (sChannels[1].on ? 2 : 0) | (sChannels[2].on ? 4 : 0) |
           (sChannels[3].on ? 8 : 0);

    RecordWav(out, count, frequency);
    if (!gPcAudioEnabled || (frequency != sStreamFrequency && !OpenStream(frequency)))
        return;
    // Keep at most ~100 ms queued so latency does not build up.
    if (SDL_GetAudioStreamQueued(sStream) < frequency * 2 / 10)
        SDL_PutAudioStreamData(sStream, out, count * 2);
}
