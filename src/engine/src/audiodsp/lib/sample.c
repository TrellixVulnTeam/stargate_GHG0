#include "audiodsp/lib/lmalloc.h"
#include "audiodsp/lib/sample.h"

#include "audiodsp/lib/interpolate-cubic.h"
#include "files.h"

#include <string.h>
#include <sndfile.h>

// Sample

enum SampleLoadError sample_init(
    struct Sample* self,
    char* path,
    SGFLT host_sr,
    int load
){
    SNDFILE *sndfile;
    SF_INFO sfinfo;
    float* data = NULL;
    float* buffer = NULL;

    if(!i_file_exists(path)){
        return SAMPLE_LOAD_FILE_NOT_FOUND;
    }

    memset(&sfinfo, 0, sizeof (sfinfo));
    sndfile = sf_open(path, SFM_READ, &sfinfo);
    if(!sndfile){
        log_error("Could not open file: %s", path);
        return SAMPLE_LOAD_READ_ERROR;
    }
    if(load){
        long length = sfinfo.frames * sfinfo.channels;
        long cline = CACHE_LINE_SIZE / sizeof(float);
        long total = length + (cline * 2);
        clalloc((void**)&data, total * sizeof(float));
        buffer = &data[cline];
        memset(data, 0, sizeof(float) * total);
        sf_count_t read_count =  sf_readf_float(
            sndfile,
            buffer,
            sfinfo.frames
        );
        sg_assert(length == read_count, "%li != %li", length, read_count);
    }
    sf_close(sndfile);
    *self = (struct Sample){
        .buffer = buffer,
        .channels = sfinfo.channels,
        .data = data,
        .frames = sfinfo.frames,
        .loaded = load,
        .ratio = (double)sfinfo.samplerate / (double)host_sr,
    };

    return SAMPLE_LOAD_NO_ERROR;
}

void sample_play_cubic(
    struct Sample* self,
    struct SamplePair* output,
    struct SampleVoice* voice
){
    output->left = cubic_interpolate_interleaved(
        self->buffer,
        self->channels,
        voice->ipos,
        voice->fpos
    );
    if(self->channels > 1){
        output->right = cubic_interpolate_interleaved(
            self->buffer,
            self->channels,
            voice->ipos + 1,
            voice->fpos
        );
    } else {
        output->right = output->left;
    }

    sample_voice_inc(voice, self);
}

// SampleVoice

void sample_voice_init(struct SampleVoice* self){
    *self = (struct SampleVoice){};
}

void sample_voice_set_rate(
    struct SampleVoice* self,
    int irate,
    SGFLT frate
){
    self->irate = irate;
    self->frate = frate;
}

void sample_voice_set_rate_pitch(
    struct SampleVoice* voice,
    struct Sample* sample,
    SGFLT pitch
){
    sg_abort("Not implemented");
}

void sample_voice_inc(struct SampleVoice* self, struct Sample* sample){
    self->fpos += self->frate;
    while(self->fpos >= 1.0){
        --self->fpos;
        ++self->ipos;
    }
    self->ipos += self->irate;
    if(self->ipos >= sample->frames){
        self->sample = NULL;
    }
}

// SampleVoices

void sample_voices_init(
    struct SampleVoices* self,
    fp_sample_play_single play_single
){
    *self = (struct SampleVoices){
        .play_single = play_single,
    };
}

struct SampleVoice* sample_voices_pick(
    struct SampleVoices* self,
    struct Sample* sample
){
    int i;
    for(i = 0; i < MAX_SAMPLE_VOICES_COUNT; ++i){
        if(!self->voices[i].sample){
            self->voices[i].sample = sample;
            self->voices[i].seq_num = self->seq_num;
            ++self->seq_num;
            ++self->active_count;
            return &self->voices[i];
        }
    }
    // TODO: Abort?  Or calling code checks for NULL?
    return NULL;
}

void sample_voices_run(
    struct SampleVoices* self,
    struct SamplePair* output
){
    int i;
    int active = 0;
    int deactivated = 0;
    struct SampleVoice* voice;
    for(i = 0; i < MAX_SAMPLE_VOICES_COUNT; ++i){
        voice = &self->voices[i];
        if(voice->sample){
            ++active;
            self->play_single(voice->sample, output, voice);
            if(!voice->sample){  // finished playing
                ++deactivated;
            }
        }
        if(active >= self->active_count){
            break;
        }
    }
    self->active_count -= deactivated;
}

