#ifndef AUDIO_BASS_BASS_BACKEND_HEAD
#define AUDIO_BASS_BASS_BACKEND_HEAD

#include "include/bass.h"
#include "include/bassmidi.h"
#include "stdlib.h"

#include <vector>

#ifndef STRDUP
    #ifdef _WIN32
        #define STRDUP _strdup
    #else
        #define STRDUP strdup
    #endif
#endif

#define VOLUME_RESOLUTION 0.003f
#define PAN_RESOLUTION 0.01f

extern bool TS_BassMidiLoaded;

enum TS_AudioDataFormat {TS_RENDERED, TS_MIDI, TS_MODULAR, TS_SAMPLED};
enum TS_AudioSimul {TS_SINGLE, TS_MULTIPLE};

int TS_InitBass();
void TS_ExplainBassErrorCode(int, bool, bool);

class TS_Sound{
public:
    TS_AudioDataFormat format;
    TS_AudioSimul simul;

    virtual ~TS_Sound() { };

    char *file;
    float volume;
    float pan;

    virtual void ResetPlayback(void) = 0;

    virtual long long GetPosition(void) const = 0;
    virtual void SetPosition(long long) = 0;

    virtual long long GetLength(void) const = 0;
    virtual bool IsPlaying(void) const = 0;

    virtual void Play(bool repeat) = 0;

    virtual void Stop(void) = 0;

    virtual void Pause(void) = 0;

    float GetVolume(void) const;
    void SetVolume(float);
    void SetVolume(unsigned char);
    float GetPan(void) const;
    void SetPan(float);
    void SetPan(short);

    bool repeat;

};

struct TS_ChannelWrap{
    DWORD handle;
    bool repeat;
    long long ID;
    TS_Sound *ptr;
};

class TS_AudioStream : public TS_Sound{
public:
    ~TS_AudioStream(void);
    //TS_AudioStream(void);
    TS_AudioStream(const char *);


    virtual void ResetPlayback(void);

    virtual void Play(bool repeat);

    virtual void Stop(void);

    virtual void Pause(void);

    virtual long long GetPosition(void) const;
    virtual void SetPosition(long long);

    virtual long long GetLength(void) const;

    virtual bool IsPlaying(void) const;

private:
    long long length;
    HSTREAM stream;
    DWORD flags;
    bool loop;
};

class TS_AudioSample : public TS_Sound{
public:

    virtual ~TS_AudioSample() = 0;

    virtual void ResetPlayback(void) = 0;

    virtual void Play(bool repeat) = 0;

    virtual void Stop(void) = 0;

    virtual void Pause(void) = 0;

    virtual long long GetPosition(void) const = 0;
    virtual void SetPosition(long long) = 0;
    virtual bool IsPlaying(void) const = 0;
    virtual long long GetLength(void) const;

private:
    long long length;
    HSAMPLE sample;   //BASS-side handle (stream, sample).
    DWORD flags;

};

class TS_AudioSampleMultiple : public TS_AudioSample{
public:
	friend void
#ifdef _MSC_VER
	__stdcall
#endif
	ChannelCallback(HSYNC handle, DWORD channel, DWORD data, void *wrapv);
    TS_AudioSampleMultiple(const char*);
    ~TS_AudioSampleMultiple(void);

    virtual void ResetPlayback(void);

    virtual void Play(bool repeat);

    virtual void Stop(void);

    virtual void Pause(void);

    virtual long long GetPosition(void) const;
    virtual void SetPosition(long long);
    virtual bool IsPlaying(void) const;

private:
    long long length;

    std::vector<TS_ChannelWrap> channels;
    HSAMPLE sample;   //BASS-side handle (stream, sample).
    DWORD flags;

};


class TS_AudioSampleSingle : public TS_AudioSample{
public:

    TS_AudioSampleSingle(const char*);
    ~TS_AudioSampleSingle(void);

    virtual void ResetPlayback(void);

    virtual void Play(bool repeat);

    virtual void Stop(void);

    virtual void Pause(void);

    virtual long long GetPosition(void) const;
    virtual void SetPosition(long long);
    virtual bool IsPlaying(void) const;
    virtual long long GetLength(void) const;

private:

    long long length;
    HCHANNEL channel;
    HSAMPLE sample;   //BASS-side handle (stream, sample).
    DWORD flags;

};

//Loading and playing sounds

//Controlling playback
void
#ifdef _MSC_VER
	__stdcall
#endif
	ChannelCallback(HSYNC handle, DWORD channel, DWORD data, void *user);
void TS_SetGlobalVolume(float volume);
void TS_ResetGlobalVolume(void);

#endif
