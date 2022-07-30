#ifndef SG_LIB_SAMPLE_H
#define SG_LIB_SAMPLE_H

#include "compiler.h"


#define MAX_SAMPLE_POOL_SIZE 2048
#define MAX_SAMPLE_VOICES_COUNT 256

enum SampleLoadError {
    SAMPLE_LOAD_NO_ERROR = 0,
    SAMPLE_LOAD_FILE_NOT_FOUND = 1,
    SAMPLE_LOAD_READ_ERROR = 2,
};

struct Sample;
struct SamplePair;
struct SampleVoice;

typedef void (*fp_sample_play_single)(
    struct Sample* self,
    struct SamplePair* output,
    struct SampleVoice* voice
);

struct Sample {
    char padding1[CACHE_LINE_SIZE];
    float* buffer;
    float* data;
    int channels;
    int frames;
    SGFLT ratio;  // playback speed, ratio of sample sr to host sr
    int end;  // index of last frame
    int loaded;
    char padding2[CACHE_LINE_SIZE];
};

// Eventual replacement for the current wav_pool, not currently in use
struct SamplePool {
    struct Sample pool[MAX_SAMPLE_POOL_SIZE];
    int count;
};

struct SampleInstancePitch {
    int note;
};

struct SampleVoice {
    struct Sample* sample;
    SGFLT last_value;
    unsigned long seq_num;
    int ipos;
    SGFLT fpos;
    int irate;
    SGFLT frate;
};

struct SampleVoices {
    char active[MAX_SAMPLE_VOICES_COUNT];  // indexes of active voices
    int active_count;
    struct SampleVoice voices[MAX_SAMPLE_VOICES_COUNT];
    int count;  // Way to artificially limit the number of voices
    unsigned long seq_num;  // Tracks age of voices
    fp_sample_play_single play_single;
};

// Sample

enum SampleLoadError sample_init(
    struct Sample* self,
    char* path,
    SGFLT host_sr,
    int load
);

void sample_play_cubic(
    struct Sample* self,
    struct SamplePair* output,
    struct SampleVoice* voice
);

// SampleVoice

void sample_voice_init(struct SampleVoice*);
void sample_voice_set_rate(struct SampleVoice* self, int irate, SGFLT frate);
void sample_voice_set_rate_pitch(
    struct SampleVoice* voice,
    struct Sample* sample,
    SGFLT pitch
);
void sample_voice_inc(struct SampleVoice* self, struct Sample* sample);

// SampleVoices

void sample_voices_init(
    struct SampleVoices*,
    fp_sample_play_single
);
struct SampleVoice* sample_voices_pick(
    struct SampleVoices* self,
    struct Sample* sample
);
void sample_voices_run(struct SampleVoices* self, struct SamplePair* output);

#endif
