// C version of the m4a sound engine's assembly (asm/lib/m4a_asm.s): the
// sequencer (MPlayMain and the ply_* commands) and the mixer (SoundMain).
// The rest of the engine is the C in src/gba/m4a.c.
//
// TMC's mixer is mono: it mixes all Direct Sound channels into one 8-bit
// buffer, which the PC platform plays together with the CGB channels.

#include "pc.h"
#include <string.h>
#include "global.h"
#include "gba/m4a_internal.h"

extern const u8 gClockTable[];
extern void* const gMPlayJumpTableTemplate[];
extern void PcAudioSubmit(const s8* samples, s32 count, s32 frequency);
extern u32 MidiKeyToFreq(WaveData* wav, u8 key, u8 fineAdjust);

#define NUM_JUMP_TABLE_ENTRIES 36

#define NEXT_CHAN(c) ((SoundChannel*)(c)->next)

// Copied to SoundMainRAM_Buffer by m4aSoundInit; holds ARM code on the GBA.
char SoundMainRAM[0x380];

u32 umul3232H32(u32 multiplier, u32 multiplicand) {
    return ((u64)multiplier * multiplicand) >> 32;
}

void MPlayJumpTableCopy(void** table) {
    memcpy(table, gMPlayJumpTableTemplate, NUM_JUMP_TABLE_ENTRIES * sizeof(void*));
}

// BIOS call 0x2A.
void MusicPlayerJumpTableCopy(void) {
    MPlayJumpTableCopy(gMPlayJumpTable);
}

// Declared without parameters in m4a_internal.h; clears 64 bytes at p.
void SoundMainBTMImpl(void* p) PC_SYMBOL(SoundMainBTM);
void SoundMainBTMImpl(void* p) {
    memset(p, 0, 64);
}

// Unlinks a channel from its track's channel list.
void RealClearChain(void* x) {
    SoundChannel* chan = x;
    MusicPlayerTrack* track = chan->track;
    SoundChannel* next;
    SoundChannel* prev;

    if (track == NULL)
        return;
    next = (SoundChannel*)chan->next;
    prev = (SoundChannel*)chan->prev;
    if (prev != NULL)
        prev->next = (u32)next;
    else
        track->chan = next;
    if (next != NULL)
        next->prev = (u32)prev;
    chan->track = NULL;
}

// Steps through a track's channel list; a channel linked to itself ends it.
static SoundChannel* NextChannel(SoundChannel* chan) {
    SoundChannel* next = NEXT_CHAN(chan);
    if (next == chan) {
        chan->next = 0;
        next = NULL;
    }
    return next;
}

// ---------------------------------------------------------------------------
// Sequencer commands

static u8 ReadByte(MusicPlayerTrack* track) {
    return *track->cmdPtr++;
}

void ply_fine(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    SoundChannel* chan = track->chan;
    while (chan != NULL) {
        if (chan->statusFlags & SOUND_CHANNEL_SF_ON)
            chan->statusFlags |= SOUND_CHANNEL_SF_STOP;
        RealClearChain(chan);
        chan = NextChannel(chan);
    }
    track->flags = 0;
}

void ply_goto(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    u8* p = track->cmdPtr;
    track->cmdPtr = (u8*)(p[0] | (p[1] << 8) | (p[2] << 16) | ((u32)p[3] << 24));
}

void ply_patt(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    if (track->patternLevel >= 3) {
        ply_fine(mplayInfo, track);
        return;
    }
    track->patternStack[track->patternLevel++] = track->cmdPtr + 4;
    ply_goto(mplayInfo, track);
}

void ply_pend(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    if (track->patternLevel != 0)
        track->cmdPtr = track->patternStack[--track->patternLevel];
}

void ply_rept(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    u8* p = track->cmdPtr;
    if (*p == 0) { // repeat forever
        track->cmdPtr = p + 1;
        ply_goto(mplayInfo, track);
        return;
    }
    track->repN++;
    if (track->repN < ReadByte(track)) {
        ply_goto(mplayInfo, track);
    } else {
        track->repN = 0;
        track->cmdPtr = p + 5;
    }
}

void ply_prio(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->priority = ReadByte(track);
}

void ply_tempo(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    mplayInfo->tempoD = ReadByte(track) << 1;
    mplayInfo->tempoI = (mplayInfo->tempoD * mplayInfo->tempoU) >> 8;
}

void ply_keysh(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->keyShift = ReadByte(track);
    track->flags |= MPT_FLG_PITCHG;
}

void ply_voice(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    memcpy(&track->tone, &mplayInfo->tone[ReadByte(track)], sizeof(ToneData));
}

void ply_vol(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->vol = ReadByte(track);
    track->flags |= MPT_FLG_VOLCHG;
}

void ply_pan(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->pan = ReadByte(track) - C_V;
    track->flags |= MPT_FLG_VOLCHG;
}

void ply_bend(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->bend = ReadByte(track) - C_V;
    track->flags |= MPT_FLG_PITCHG;
}

void ply_bendr(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->bendRange = ReadByte(track);
    track->flags |= MPT_FLG_PITCHG;
}

void ply_lfos(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->lfoSpeed = ReadByte(track);
    if (track->lfoSpeed == 0)
        ClearModM(track);
}

void ply_lfodl(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->lfoDelay = ReadByte(track);
}

void ply_mod(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->mod = ReadByte(track);
    if (track->mod == 0)
        ClearModM(track);
}

void ply_modt(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    u8 modT = ReadByte(track);
    if (track->modT != modT) {
        track->modT = modT;
        track->flags |= MPT_FLG_VOLCHG | MPT_FLG_PITCHG;
    }
}

void ply_tune(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    track->tune = ReadByte(track) - C_V;
    track->flags |= MPT_FLG_PITCHG;
}

// Writes a CGB sound register.
void ply_port(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    u8 reg = ReadByte(track);
    *(vu8*)(REG_BASE + 0x60 + reg) = ReadByte(track);
}

void ply_endtie(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    SoundChannel* chan;
    u8 key;

    if (*track->cmdPtr < 0x80)
        track->key = ReadByte(track);
    key = track->key;
    for (chan = track->chan; chan != NULL; chan = NextChannel(chan)) {
        if ((chan->statusFlags & (SOUND_CHANNEL_SF_START | SOUND_CHANNEL_SF_ENV)) &&
            !(chan->statusFlags & SOUND_CHANNEL_SF_STOP) && chan->midiKey == key) {
            chan->statusFlags |= SOUND_CHANNEL_SF_STOP;
            return;
        }
    }
}

void TrackStop(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    SoundChannel* chan;

    if (!(track->flags & MPT_FLG_EXIST))
        return;
    for (chan = track->chan; chan != NULL; chan = NextChannel(chan)) {
        if (chan->statusFlags != 0) {
            if (chan->type & TONEDATA_TYPE_CGB)
                SOUND_INFO_PTR->CgbOscOff(chan->type & TONEDATA_TYPE_CGB);
            chan->statusFlags = 0;
        }
        chan->track = NULL;
    }
    track->chan = NULL;
}

static void ChnVolSet(SoundChannel* chan, MusicPlayerTrack* track) {
    s32 pan = (s8)chan->rhythmPan;
    u32 v = (chan->velocity * (0x80 + pan) * track->volMR) >> 14;
    chan->rightVolume = v > 0xFF ? 0xFF : v;
    v = (chan->velocity * (0x7F - pan) * track->volML) >> 14;
    chan->leftVolume = v > 0xFF ? 0xFF : v;
}

// Picks a Direct Sound channel for a new note: a free one, else the stopping
// or (if none is stopping) playing one with the lowest priority.
static SoundChannel* AllocDirectSoundChannel(SoundInfo* soundInfo, MusicPlayerTrack* track, u32 priority) {
    SoundChannel* chan = soundInfo->chans;
    SoundChannel* best = NULL;
    u32 bestPriority = priority;
    MusicPlayerTrack* bestTrack = track;
    int foundStopping = FALSE;
    s32 i;

    for (i = soundInfo->maxChannels; i > 0; i--, chan++) {
        if (!(chan->statusFlags & SOUND_CHANNEL_SF_ON))
            return chan;
        if (chan->statusFlags & SOUND_CHANNEL_SF_STOP) {
            if (!foundStopping) {
                foundStopping = TRUE;
                bestPriority = chan->priority;
                bestTrack = chan->track;
                best = chan;
                continue;
            }
        } else if (foundStopping) {
            continue;
        }
        if (chan->priority < bestPriority) {
            bestPriority = chan->priority;
            bestTrack = chan->track;
            best = chan;
        } else if (chan->priority == bestPriority && (uintptr_t)chan->track >= (uintptr_t)bestTrack) {
            bestTrack = chan->track;
            best = chan;
        }
    }
    return best;
}

// Called through SoundInfo.plynote with the note length index first, unlike
// its declaration in m4a_internal.h.
void PlyNote(u32 clockIndex, MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) PC_SYMBOL(ply_note);
void PlyNote(u32 clockIndex, MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track) {
    SoundInfo* soundInfo = SOUND_INFO_PTR;
    ToneData* tone;
    SoundChannel* chan;
    u32 key, priority, cgbType;
    s32 rhythmPan = 0, finalKey;
    u8* p;

    track->gateTime = gClockTable[clockIndex];
    p = track->cmdPtr;
    if (*p < 0x80) {
        track->key = *p++;
        if (*p < 0x80) {
            track->velocity = *p++;
            if (*p < 0x80)
                track->gateTime += *p++;
        }
        track->cmdPtr = p;
    }

    tone = &track->tone;
    key = track->key;
    if (tone->type & (TONEDATA_TYPE_RHY | TONEDATA_TYPE_SPL)) {
        u32 index = key;
        if (tone->type & TONEDATA_TYPE_SPL)
            index = ((u8*)(uptr) * (u32*)&tone->attack)[key]; // key split table
        tone = (ToneData*)(uptr)tone->wav + index;
        if (tone->type & (TONEDATA_TYPE_RHY | TONEDATA_TYPE_SPL))
            return;
        if (track->tone.type & TONEDATA_TYPE_RHY) {
            if (tone->pan_sweep & 0x80)
                rhythmPan = ((s32)tone->pan_sweep - 0xC0) * 2;
            key = tone->key;
        }
    }

    priority = mplayInfo->priority + track->priority;
    if (priority > 0xFF)
        priority = 0xFF;
    cgbType = tone->type & TONEDATA_TYPE_CGB;

    if (cgbType != 0) {
        if (soundInfo->cgbChannels == NULL)
            return;
        chan = (SoundChannel*)&soundInfo->cgbChannels[cgbType - 1];
        if ((chan->statusFlags & SOUND_CHANNEL_SF_ON) && !(chan->statusFlags & SOUND_CHANNEL_SF_STOP)) {
            if (chan->priority > priority)
                return;
            if (chan->priority == priority && (uintptr_t)chan->track < (uintptr_t)track)
                return;
        }
    } else {
        chan = AllocDirectSoundChannel(soundInfo, track, priority);
        if (chan == NULL)
            return;
    }

    ClearChain(chan);
    chan->prev = 0;
    chan->next = (u32)track->chan;
    if (track->chan != NULL)
        track->chan->prev = (u32)chan;
    track->chan = chan;
    chan->track = track;

    track->lfoDelayC = track->lfoDelay;
    if (track->lfoDelay != 0)
        ClearModM(track);
    TrkVolPitSet(mplayInfo, track);

    chan->gateTime = track->gateTime;
    chan->midiKey = track->key;
    chan->velocity = track->velocity;
    chan->priority = priority;
    chan->key = key;
    chan->rhythmPan = rhythmPan;
    chan->type = tone->type;
    chan->wav = (WaveData*)(uptr)tone->wav;
    chan->attack = tone->attack;
    chan->decay = tone->decay;
    chan->sustain = tone->sustain;
    chan->release = tone->release;
    chan->echoVolume = track->echoVolume;
    chan->echoLength = track->echoLength;
    ChnVolSet(chan, track);

    finalKey = chan->key + track->keyM;
    if (finalKey < 0)
        finalKey = 0;
    if (cgbType != 0) {
        CgbChannel* cgb = (CgbChannel*)chan;
        u8 sweep = tone->pan_sweep;
        cgb->length = tone->length;
        cgb->sweep = (sweep & 0x80) || !(sweep & 0x70) ? 8 : sweep;
        chan->frequency = soundInfo->MidiKeyToCgbFreq(cgbType, finalKey, track->pitM);
    } else {
        chan->frequency = MidiKeyToFreq(chan->wav, finalKey, track->pitM);
    }
    chan->statusFlags = SOUND_CHANNEL_SF_START;
    track->flags &= 0xF0;
}

// ---------------------------------------------------------------------------
// Sequencer

static void UpdateTrackLfo(MusicPlayerTrack* track) {
    s32 phase, modM;

    if (track->lfoSpeed == 0 || track->mod == 0)
        return;
    if (track->lfoDelayC != 0) {
        track->lfoDelayC--;
        return;
    }
    track->lfoSpeedC += track->lfoSpeed;
    phase = track->lfoSpeedC;
    // Triangle wave from the phase.
    if ((s8)(phase - 0x40) < 0)
        modM = (s8)phase;
    else
        modM = 0x80 - phase;
    modM = (track->mod * modM) >> 6;
    if ((u8)(modM ^ track->modM) != 0) {
        track->modM = modM;
        track->flags |= track->modT == 0 ? MPT_FLG_PITCHG : MPT_FLG_VOLCHG;
    }
}

static void RunTrackCommands(MusicPlayerInfo* mplayInfo, MusicPlayerTrack* track, SoundInfo* soundInfo) {
    while (track->wait == 0) {
        u32 cmd = *track->cmdPtr;
        if (cmd < 0x80) {
            cmd = track->runningStatus;
        } else {
            track->cmdPtr++;
            if (cmd >= 0xBD)
                track->runningStatus = cmd;
        }
        if (cmd >= 0xCF) {
            ((void (*)(u32, MusicPlayerInfo*, MusicPlayerTrack*))soundInfo->plynote)(cmd - 0xCF, mplayInfo, track);
        } else if (cmd > 0xB0) {
            mplayInfo->cmd = cmd - 0xB1;
            ((XcmdFunc*)soundInfo->MPlayJumpTable)[mplayInfo->cmd](mplayInfo, track);
            if (track->flags == 0)
                return;
        } else {
            track->wait = gClockTable[cmd - 0x80];
        }
    }
    track->wait--;
    UpdateTrackLfo(track);
}

void MPlayMain(MusicPlayerInfo* mplayInfo) {
    SoundInfo* soundInfo = SOUND_INFO_PTR;
    MusicPlayerTrack* track;
    u32 tempoC;
    s32 i;

    if (mplayInfo->ident != ID_NUMBER)
        return;
    mplayInfo->ident++; // lock
    if (mplayInfo->func != NULL)
        mplayInfo->func((MusicPlayerInfo*)mplayInfo->intp); // next music player in the chain

    if (!(mplayInfo->status & MUSICPLAYER_STATUS_PAUSE))
        FadeOutBody(mplayInfo);
    if (mplayInfo->status & MUSICPLAYER_STATUS_PAUSE)
        goto done;

    tempoC = mplayInfo->tempoC + mplayInfo->tempoI;
    while (mplayInfo->tempoC = tempoC, tempoC >= 150) {
        u32 bit = 1, active = 0;

        track = mplayInfo->tracks;
        for (i = mplayInfo->trackCount; i > 0; i--, track++, bit <<= 1) {
            SoundChannel* chan;

            if (!(track->flags & MPT_FLG_EXIST))
                continue;
            active |= bit;
            for (chan = track->chan; chan != NULL; chan = NextChannel(chan)) {
                if (!(chan->statusFlags & SOUND_CHANNEL_SF_ON)) {
                    ClearChain(chan);
                } else if (chan->gateTime != 0 && --chan->gateTime == 0) {
                    chan->statusFlags |= SOUND_CHANNEL_SF_STOP;
                }
            }
            if (track->flags & MPT_FLG_START) {
                ClearTrackState(track);
                track->flags = MPT_FLG_EXIST;
                track->bendRange = 2;
                track->volX = 64;
                track->lfoSpeed = 22;
                track->tone.type = 1;
            }
            RunTrackCommands(mplayInfo, track, soundInfo);
        }
        mplayInfo->clock++;
        if (active == 0) {
            mplayInfo->status = MUSICPLAYER_STATUS_PAUSE;
            goto done;
        }
        mplayInfo->status = active;
        tempoC = mplayInfo->tempoC - 150;
    }

    // Apply volume and pitch changes to the playing channels.
    track = mplayInfo->tracks;
    for (i = mplayInfo->trackCount; i > 0; i--, track++) {
        SoundChannel* chan;

        if (!(track->flags & MPT_FLG_EXIST) || !(track->flags & 0xF))
            continue;
        TrkVolPitSet(mplayInfo, track);
        for (chan = track->chan; chan != NULL; chan = NextChannel(chan)) {
            u32 cgbType;
            s32 key;

            if (!(chan->statusFlags & SOUND_CHANNEL_SF_ON)) {
                ClearChain(chan);
                continue;
            }
            cgbType = chan->type & TONEDATA_TYPE_CGB;
            if (track->flags & MPT_FLG_VOLCHG) {
                ChnVolSet(chan, track);
                if (cgbType != 0)
                    ((CgbChannel*)chan)->modify |= CGB_CHANNEL_MO_VOL;
            }
            if (track->flags & MPT_FLG_PITCHG) {
                key = chan->key + track->keyM;
                if (key < 0)
                    key = 0;
                if (cgbType != 0) {
                    chan->frequency = soundInfo->MidiKeyToCgbFreq(cgbType, key, track->pitM);
                    ((CgbChannel*)chan)->modify |= CGB_CHANNEL_MO_PIT;
                } else {
                    chan->frequency = MidiKeyToFreq(chan->wav, key, track->pitM);
                }
            }
        }
        track->flags &= 0xF0;
    }

done:
    mplayInfo->ident = ID_NUMBER;
}

// ---------------------------------------------------------------------------
// Mixer

// Steps a channel's envelope. Returns FALSE if the channel stopped.
static bool32 UpdateEnvelope(SoundInfo* soundInfo, SoundChannel* chan) {
    u8 sf = chan->statusFlags;
    s32 env;

    if (sf & SOUND_CHANNEL_SF_START) {
        if (sf & SOUND_CHANNEL_SF_STOP) {
            chan->statusFlags = 0;
            return FALSE;
        }
        sf = SOUND_CHANNEL_SF_ENV_ATTACK;
        chan->currentPointer = (u32)chan->wav->data;
        chan->count = chan->wav->size;
        chan->fw = 0;
        env = 0;
        if (chan->wav->status & 0xC000) // the high status byte holds the loop flag
            sf |= SOUND_CHANNEL_SF_LOOP;
        goto attack;
    }

    env = chan->envelopeVolume;
    if (sf & SOUND_CHANNEL_SF_IEC) {
        if (--chan->echoLength == 0) {
            chan->statusFlags = 0;
            return FALSE;
        }
    } else if (sf & SOUND_CHANNEL_SF_STOP) {
        env = (env * chan->release) >> 8;
        if (env <= chan->echoVolume) {
        echo:
            env = chan->echoVolume;
            if (env == 0) {
                chan->statusFlags = 0;
                return FALSE;
            }
            sf |= SOUND_CHANNEL_SF_IEC;
        }
    } else if ((sf & SOUND_CHANNEL_SF_ENV) == SOUND_CHANNEL_SF_ENV_DECAY) {
        env = (env * chan->decay) >> 8;
        if (env <= chan->sustain) {
            env = chan->sustain;
            if (env == 0)
                goto echo;
            sf--;
        }
    } else if ((sf & SOUND_CHANNEL_SF_ENV) == SOUND_CHANNEL_SF_ENV_ATTACK) {
    attack:
        env += chan->attack;
        if (env >= 0xFF) {
            env = 0xFF;
            sf--;
        }
    }
    chan->statusFlags = sf;
    chan->envelopeVolume = env;
    env = ((soundInfo->masterVolume + 1) * env) >> 4;
    chan->envelopeVolumeRight = ((chan->rightVolume + chan->leftVolume) * env) >> 9;
    return TRUE;
}

static void MixChannel(SoundInfo* soundInfo, SoundChannel* chan, s8* out, s32 samples) {
    s32 vol = chan->envelopeVolumeRight;
    s8* p = (s8*)chan->currentPointer;
    s32 count = chan->count;
    s8* loopStart = NULL;
    s32 loopLength = 0;
    s32 i;

    if (chan->statusFlags & SOUND_CHANNEL_SF_LOOP) {
        loopStart = chan->wav->data + chan->wav->loopStart;
        loopLength = chan->wav->size - chan->wav->loopStart;
    }

    if (chan->type & TONEDATA_TYPE_FIX) {
        // Played at the mixing rate.
        for (i = 0; i < samples; i++) {
            out[i] += (*p++ * vol) >> 8;
            if (--count == 0) {
                if (loopLength == 0) {
                    chan->statusFlags = 0;
                    return;
                }
                p = loopStart;
                count = loopLength;
            }
        }
    } else {
        // Resampled with linear interpolation; fw is a 23-bit fraction.
        u32 fw = chan->fw;
        u32 step = soundInfo->divFreq * chan->frequency;
        s32 s0 = *p++;
        s32 delta = *p - s0;

        for (i = 0; i < samples; i++) {
            s32 sample = s0 + ((s32)(fw * (u32)delta) >> 23);
            s32 advance;

            out[i] += (sample * vol) >> 8;
            fw += step;
            advance = fw >> 23;
            if (advance == 0)
                continue;
            fw &= ~0x3F800000u;
            count -= advance;
            if (count <= 0) {
                s32 offset;
                if (loopLength == 0) {
                    chan->statusFlags = 0;
                    return;
                }
                offset = -count;
                for (;;) {
                    count += loopLength;
                    if (count > 0)
                        break;
                    offset -= loopLength;
                }
                p = loopStart + offset;
                s0 = *p++;
            } else if (advance == 1) {
                s0 += delta;
                p++;
            } else {
                p += advance - 1;
                s0 = *p++;
            }
            delta = *p - s0;
        }
        p--;
        chan->fw = fw;
    }
    chan->count = count;
    chan->currentPointer = (u32)p;
}

void SoundMain(void) {
    SoundInfo* soundInfo = SOUND_INFO_PTR;
    s32 samples, i;
    s8* out;

    if (soundInfo->ident != ID_NUMBER)
        return;
    soundInfo->ident++; // lock

    if (soundInfo->MPlayMainHead != NULL)
        soundInfo->MPlayMainHead((MusicPlayerInfo*)soundInfo->intp);
    soundInfo->CgbSound();

    // The DMA ring buffer holds pcmDmaPeriod frames; mix the next one.
    samples = soundInfo->pcmSamplesPerVBlank;
    out = soundInfo->pcmBuffer;
    if (soundInfo->pcmDmaCounter > 1)
        out += (soundInfo->pcmDmaPeriod - (soundInfo->pcmDmaCounter - 1)) * samples;

    if (soundInfo->reverb != 0) {
        // Mix in the previous pass over this part of the ring.
        const s8* other = soundInfo->pcmDmaCounter == 2 ? soundInfo->pcmBuffer : out + samples;
        for (i = 0; i < samples; i++) {
            s32 v = ((out[i] + other[i]) * soundInfo->reverb) >> 8;
            if (v & 0x80)
                v++;
            out[i] = v;
        }
    } else {
        memset(out, 0, samples);
    }

    for (i = 0; i < soundInfo->maxChannels; i++) {
        SoundChannel* chan = &soundInfo->chans[i];
        if (!(chan->statusFlags & SOUND_CHANNEL_SF_ON))
            continue;
        if (!UpdateEnvelope(soundInfo, chan))
            continue;
        MixChannel(soundInfo, chan, out, samples);
    }

    PcAudioSubmit(out, samples, soundInfo->pcmFreqency);
    soundInfo->ident = ID_NUMBER;
}
