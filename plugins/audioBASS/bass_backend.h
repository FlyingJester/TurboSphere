#ifndef AUDIO_BASS_BASS_BACKEND_HEAD
#define AUDIO_BASS_BASS_BACKEND_HEAD

#include "include/bass.h"
#include "include/bassmidi.h"
#include "stdlib.h"

#ifndef STRDUP
    #ifdef _WIN32
        #define STRDUP _strdup
    #else
        #define STRDUP strdup
    #endif
#endif

#define VOLUME_RESOLUTION 0.003f
#define PAN_RESOLUTION 0.01f

enum TS_AudioType {TS_STREAM, TS_SAMPLE, TS_VIRTUALOUT};

typedef DWORD TS_SoundHandle;

int TS_InitBass();
void TS_ExplainBassErrorCode(int, bool, bool);

class TS_AudioSample{
public:
    TS_AudioSample(void);
    TS_AudioSample(const char*);
    ~TS_AudioSample(void);

    TS_AudioType type;

    DWORD handle;   //BASS-side handle (stream, sample).
    char *file; //Filename.
    int playbacks;    //Number of simultaneous playbacks.
    DWORD flags;
    float volume;
    float pan;

    float GetVolume(void);
    void SetVolume(float);
    void SetVolume(unsigned char);
    float GetPan(void);
    void SetPan(float);
    void SetPan(unsigned char);
};

class TS_AudioChannel{
public:
    TS_AudioChannel(void);
    TS_AudioChannel(TS_AudioSample*);
    ~TS_AudioChannel(void);
    HCHANNEL channel;  //BASS-side channel.
    TS_AudioSample *sample;
    BOOL repeat;       //Should this channel be on repeat?

    void Play(int);
};

//Loading and playing sounds
void TS_PlaySound(TS_AudioSample);

//Controlling playback
void TS_SetGlobalVolume(float volume);
void TS_ResetGlobalVolume(void);

float fabs(float f);


void TS_PlaySound(TS_AudioSample *sample, bool loop);
void TS_PlaySoundEffect(TS_AudioSample *sample);
TS_AudioChannel *TS_CreateChannel(TS_AudioSample *sample);
void TS_MakeSampleSound(TS_AudioSample *sample);
void TS_MakeSampleSoundEffect(TS_AudioSample *sample);
void TS_RestartChannel(TS_AudioChannel*);
#endif
