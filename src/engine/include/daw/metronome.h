#ifndef METRONOME_H
#define METRONOME_H

#include "compiler.h"

#include "audiodsp/lib/sample.h"

struct Metronome {
    struct Sample samples[3];
    struct SampleVoices voices;
};

void metronome_init(struct Metronome*, SGFLT);
void metronome_run(
    struct Metronome* self,
    struct SamplePair* output,
    double start_beat,
    double end_beat,
    int sample_count
);

#endif
