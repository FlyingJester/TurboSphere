#include "include/bass.h"
#include "frontend.h"
#include "bass_backend.h"
#include <cmath>
#include <cassert>
#include <cstdint>
#include "../../common/literaltype.h"

//TODO: Add seperate error for OS X.
#ifdef _WIN32
static const char *TS_MSG_DXError = "DirectX is not installed.\n";
#define fabs TS_FABS
#else
static const char *TS_MSG_DXError = "ALSA is not installed.";
#endif

static DWORD *defaultSoundFont;

//TODO: Perhaps having a T5 file (and even localizations) would be a good idea?
static const char *TS_MSG_WillReopen    = "\tTurboSphere will attempt to close and reopen the device.";
static const char *TS_MSG_WillChangeFmt = "\tTurboSphere will attempt to start audio with different settings.";
static const char *TS_MSG_BadDevice     = "Invalid device. This probably means you have bad drivers, or no working audio output devices.";
static const char *TS_MSG_AlreadyOpen   = "The device has already been initialized.";
static const char *TS_MSG_BadDriver     = "There is no available device driver.";
static const char *TS_MSG_BadFormat     = "The default default format is not supported by the device.";
static const char *TS_MSG_BadFormatFS   = "The default failsafe format is not supported by the device.";
static const char *TS_MSG_OutOfMemory   = "Out of memory.";
static const char *TS_MSG_UnkownErrorPRT= "Unkown error (internal code is [decimal] %i).";

uint64_t generateUniqueId(int skip = 0){
    static uint64_t ID = 0;
    ID+=skip;
    return ID++;
}

void TS_ExplainBassErrorCode(int error, bool willTryAgain, bool FormatIsFailsafe){

    printf("[" PLUGINNAME "] Error: Could not initialize BASS. ");

    switch(error){
        case BASS_ERROR_DX:
            printf(TS_MSG_DXError);
            break;
        case BASS_ERROR_DEVICE:
            printf(TS_MSG_BadDevice);
            break;
        case BASS_ERROR_ALREADY:
            printf(TS_MSG_AlreadyOpen);
            if(willTryAgain){
                printf("\n");
                printf(TS_MSG_WillReopen);
            }
            break;
        case BASS_ERROR_DRIVER:
            printf(TS_MSG_BadDriver);
            if(willTryAgain){
                printf("\n");
                printf(TS_MSG_WillReopen);
            }
            break;
        case BASS_ERROR_FORMAT:
            if(!FormatIsFailsafe){
                printf(TS_MSG_BadFormat);
            }
            else{
                printf(TS_MSG_BadFormatFS);
            }
            if(willTryAgain){
                printf("\n");
                printf(TS_MSG_WillChangeFmt);
            }
        case BASS_ERROR_MEM:
            printf(TS_MSG_OutOfMemory);
            break;
        default:
            printf(TS_MSG_UnkownErrorPRT, error);
    }
    printf("\n");
}

int TS_InitBassMidi(){

    TS_Config *TS_Conf = GetConfig();
    TS_Directories *TS_Dirs = GetDirs();

    HPLUGIN bassmidi;
    #ifdef _WIN32
        bassmidi = BASS_PluginLoad("bassmidi.dll", 0);
    #elif defined(__linux__)
        bassmidi = BASS_PluginLoad("libbassmidi.so", 0);
    #else // OSX?
        bassmidi = BASS_PluginLoad("libbassmidi.dylib", 0);
    #endif
    bool bassMidiLoaded = true;
    int error = BASS_ErrorGetCode();
    if(error){
        printf("[" PLUGINNAME "] Warning: Could not initialize BASSmidi.");
        printf(TS_MSG_UnkownErrorPRT, error);
        printf("\n");
        bassMidiLoaded = false;
    }
    else{
        printf("[" PLUGINNAME "] Info: Initialized BASSmidi as bass plugin %i.\n", bassmidi);
    }
    defaultSoundFont = (DWORD *)malloc(sizeof(DWORD));

    *defaultSoundFont = BASS_MIDI_FontInit(string(TS_Dirs->system).append(TS_Conf->systemsoundfont).c_str(), 0);

    BASS_MIDI_FONT defaultFont;

    BASS_MIDI_StreamGetFonts(0, &defaultFont, 1);

    defaultFont.font   = *defaultSoundFont;
    defaultFont.preset = -1;
    defaultFont.bank   = 0;

    if(!BASS_MIDI_StreamSetFonts(0, &defaultFont, 1))
        bassMidiLoaded = false;

    if(!bassMidiLoaded){
        printf("[" PLUGINNAME "] Warning: Could not initialize MIDI support. Sound will still function, but MIDI files will not play.\n");
    }

    error = BASS_ErrorGetCode();
    if((!bassMidiLoaded)||(error)){
        printf("[" PLUGINNAME "] Warning: Could not load the default sound font. ");
        switch(error){
            case BASS_ERROR_HANDLE:
                 printf("Check that the file exists.");
                break;
            case BASS_ERROR_CODEC:
                printf("No codec exists for the samples.");
                break;
            case BASS_ERROR_NOTAVAIL:
                printf("The sound font does not contain necessary samples.");
                break;
            default:
                printf(TS_MSG_UnkownErrorPRT, error);
                break;
        }
        printf("\n");
    }

    BASS_SetConfig(BASS_CONFIG_MIDI_VOICES, 200);

    return 0;
}

int TS_InitBass(){

    DWORD version = BASS_GetVersion();

    if(HIWORD(version)!=BASSVERSION){
        printf("[" PLUGINNAME "] Warning: Wrong version of BASS loaded (expected %x, version was %x). Crashes will likely follow.\n", BASSVERSION, version);
    }

    TS_InitBassMidi();
	if (!BASS_Init(-1,44100,BASS_DEVICE_FREQ,NULL,NULL)) {
	    int error = BASS_ErrorGetCode();
        TS_ExplainBassErrorCode(error, true, false);

        BOOL couldstart = 1;

        if((error==BASS_ERROR_ALREADY)||(error==BASS_ERROR_ALREADY)){
            BASS_Free();
            couldstart = BASS_Init(-1, 44100, BASS_DEVICE_FREQ, NULL, NULL);
            error = BASS_ErrorGetCode();
            if(error==BASS_ERROR_FORMAT){
                BASS_Free();
            }
        }

        if(error==BASS_ERROR_FORMAT){
            BASS_Init(-1, 22050, FAILSAFE_BASS_INIT_FLAGS|BASS_DEVICE_FREQ, NULL, NULL);
        }

        if(!couldstart){
            error = BASS_ErrorGetCode();

            TS_ExplainBassErrorCode(error, false, (error==BASS_ERROR_FORMAT)?true:false);
        return 1;
        }


	}
	else{
        printf("[" PLUGINNAME "] Info: loaded and initialized BASS.\n");
	}
    BASS_Start();
    return 0;
}


/*

class TS_Sound{
public:
    TS_AudioDataFormat format;
    TS_AudioSimul simul;

    virtual ~TS_Sound() = 0;

    char *file;
    float volume;
    float pan;

    virtual void ResetPlayback(void) const = 0;

    virtual long long GetPosition(void) const = 0;
    virtual void SetPosition(long long) = 0;

    virtual long long GetLength(void) const = 0;
    virtual bool IsPlaying(void) const = 0;

    virtual void Play(bool repeat) = 0;

    virtual void Stop(void) = 0;

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
    uint64_t ID;
    TS_Sound *ptr;
};

*/

float TS_Sound::GetVolume() const{
    return volume;
}

float TS_Sound::GetPan() const{
    return pan;
}

void TS_Sound::SetVolume(unsigned char v){
    SetVolume((float)((float)v/255.0f));
}

void TS_Sound::SetVolume(float v){
    if(v>1.0f){
        v=1.0f;
    }
    else if(v<-1.0f){
        v=-1.0f;
    }
    volume = v;
}

void TS_Sound::SetPan(short p){
    if(p>255){
        p=255;
    }
    else if(p<-256){
        p=256;
    }
    SetPan((float)((float)p/255.0f));
}

void TS_Sound::SetPan(float p){
    if(p>1.0f){
        p=1.0f;
    }
    else if(p<-1.0f){
        p=-1.0f;
    }
    pan = p;
}

/*

class TS_AudioStream : public TS_Sound{
public:
    ~TS_AudioStream(void);
    TS_AudioStream(void);
    TS_AudioStream(const char *, TS_AudioSimul);


    virtual void ResetPlayback(void) const;

    virtual void Play(bool repeat);

    virtual void Stop(void);

    virtual void Pause(void);

    virtual long long GetPosition(void) const;
    virtual void SetPosition(long long);
    virtual bool IsPlaying(void) const;

    virtual long long GetLength(void) const;

private:
    HSTREAM stream;
    DWORD flags;
    bool loop;
};

*/

TS_AudioStream::TS_AudioStream(const char *f){

    stream =  BASS_MIDI_StreamCreateFile(0, f, 0, 0, BASS_MIDI_DECAYEND|BASS_MIDI_NOCROP|BASS_MIDI_SINCINTER, 0);
    int error = BASS_ErrorGetCode();
    if(error)
        stream = BASS_StreamCreateFile(0, f, 0, 0, BASS_STREAM_PRESCAN);
}

TS_AudioStream::~TS_AudioStream(){
    BASS_StreamFree(stream);
}

void TS_AudioStream::ResetPlayback(){
    BASS_ChannelStop(stream);
    BASS_ChannelPlay(stream, loop);
}

void TS_AudioStream::Play(bool repeat){
    loop = repeat;
    ResetPlayback();
}

void TS_AudioStream::Stop(){
    BASS_ChannelSetPosition(stream, 0, BASS_POS_BYTE);
    BASS_ChannelStop(stream);
}

void TS_AudioStream::Pause(){
    BASS_ChannelPause(stream);
}

long long TS_AudioStream::GetPosition() const{
        long long pos = BASS_ChannelGetPosition(stream, BASS_POS_BYTE);
        double dlength = BASS_ChannelBytes2Seconds(stream, pos);
        return (long long)(dlength*DOUBLE(1000.0));
}

void TS_AudioStream::SetPosition(long long position){
    BASS_ChannelSetPosition(stream, BASS_ChannelSeconds2Bytes(stream, (double)(position)/DOUBLE_LITERAL(1000.0)), BASS_POS_BYTE);
}

long long TS_AudioStream::GetLength() const{
        long long end = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
        double dlength = BASS_ChannelBytes2Seconds(stream, end);
        return (long long)(dlength*DOUBLE(1000.0));
}

bool TS_AudioStream::IsPlaying() const{
    return (bool)BASS_ChannelIsActive(stream);
}

/*

class TS_AudioSample : public TS_Sound{
public:

    virtual ~TS_AudioSample() = 0;

    virtual void ResetPlayback(void) const = 0;

    virtual void Play(bool repeat) = 0;

    virtual void Stop(void) = 0;

    virtual void Pause(void) = 0;

    virtual long long GetPosition(void) const = 0;
    virtual void SetPosition(long long) = 0;
    virtual bool IsPlaying(void) const = 0;
    virtual long long GetLength(void) const;
};

*/

long long TS_AudioSample::GetLength() const{
    HCHANNEL channel = BASS_SampleGetChannel(sample, 0);
    long long end = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
    double dlength = BASS_ChannelBytes2Seconds(channel, end);
    return (long long)(dlength*DOUBLE_LITERAL(1000.0));
}

TS_AudioSample::~TS_AudioSample(){
    BASS_SampleFree(sample);
}

/*

class TS_AudioSampleMultiple : public TS_AudioSample{

    TS_AudioSampleMultiple(const char*);
    ~TS_AudioSampleMultiple(void);

    virtual void ResetPlayback(void) const;

    virtual void Play(bool repeat);

    virtual void Stop(void);

    virtual long long GetPosition(void) const;
    virtual void SetPosition(long long);
    virtual bool IsPlaying(void) const;

private:
    std::vector<TS_ChannelWrap> channels;
    HSAMPLE sample;   //BASS-side handle (stream, sample).
    DWORD flags;
};

struct TS_ChannelWrap{
    DWORD handle;
    bool repeat;
    uint64_t ID;
    TS_Sound *ptr;
};
*/
void
#ifdef _MSC_VER
__stdcall 
#endif
	ChannelCallback(HSYNC handle, DWORD channel, DWORD data, void *wrapv){
	TS_ChannelWrap *wrap = (TS_ChannelWrap *)wrapv;
    TS_AudioSampleMultiple *sound = (TS_AudioSampleMultiple *)(wrap->ptr);
    printf("A channel has finished. Channel number is %i.\n", channel);

}

TS_AudioSampleMultiple::TS_AudioSampleMultiple(const char *f){
    sample = BASS_SampleLoad(0, f, 0, 0, 16, 0);
}

void TS_AudioSampleMultiple::ResetPlayback(void){

}

TS_AudioSampleMultiple::~TS_AudioSampleMultiple(void){

}

void TS_AudioSampleMultiple::Play(bool repeat){

    int highestIndex = 0;
    uint64_t highestID = 0;
    bool foundPaused = false;
    for(size_t i = 1; i<channels.size(); i++){
        if((BASS_ChannelIsActive(channels[i].handle) == BASS_ACTIVE_PAUSED)&&(channels[i].ID>highestID)){
            highestID = channels[i].ID;
            highestIndex = i;
            foundPaused = true;
        }
    }

    if(foundPaused){
        BASS_ChannelPlay(channels[highestIndex].handle, repeat);
        return;
    }

    TS_ChannelWrap *wrap = (TS_ChannelWrap *)malloc(sizeof(TS_ChannelWrap));
    wrap->ID = generateUniqueId(0);
    wrap->handle = BASS_SampleGetChannel(sample, 0);
    wrap->ptr = this;
    wrap->repeat = repeat;
    if(!repeat){
        BASS_ChannelSetSync(wrap->handle, BASS_SYNC_END, 0, ChannelCallback, (void *)wrap);
    }
}

void TS_AudioSampleMultiple::Pause(void){

    int highestIndex = 0;
    uint64_t highestID = 0;
    bool foundPlaying = false;
    for(size_t i = 1; i<channels.size(); i++){
        if(((BASS_ChannelIsActive(channels[i].handle) == BASS_ACTIVE_PLAYING)||(BASS_ChannelIsActive(channels[i].handle) == BASS_ACTIVE_STALLED))&&(channels[i].ID>highestID)){
            highestID = channels[i].ID;
            highestIndex = i;
            foundPlaying = true;
        }
    }
    if(foundPlaying){
        BASS_ChannelPause(channels[highestIndex].handle);
    }
}

void TS_AudioSampleMultiple::Stop(void){
    for(size_t i = 0; i<channels.size(); i++){
        BASS_ChannelStop(channels[i].handle);
    }
    channels.resize(0);
}

long long TS_AudioSampleMultiple::GetPosition(void) const{
    return 0;
}

void TS_AudioSampleMultiple::SetPosition(long long p){
    if(channels.size()==0){
        Play(false);
        SetPosition(p);
        Pause();
    }

    int highestIndex = 0;
    uint64_t highestID = channels[0].ID;
    for(size_t i = 1; i<channels.size(); i++){
        if(channels[i].ID>highestID){
            highestID = channels[i].ID;
            highestIndex = i;
        }
    }

    BASS_ChannelSetPosition(channels[highestIndex].handle, BASS_ChannelSeconds2Bytes(channels[highestIndex].handle, (double)(p)/DOUBLE_LITERAL(1000.0)), BASS_POS_BYTE);
}

bool TS_AudioSampleMultiple::IsPlaying(void) const{
    return (channels.size()>0);
}

/*
class TS_AudioSampleSingle : public TS_AudioSample{

    TS_AudioSampleSingle(const char*);
    ~TS_AudioSampleSingle(void);

    virtual void ResetPlayback(void) const;

    virtual void Play(bool repeat);

    virtual void Stop(void);

    virtual long long GetPosition(void) const;
    virtual void SetPosition(long long);
    virtual bool IsPlaying(void) const;

private:
    HCHANNEL channel;
    HSAMPLE sample;   //BASS-side handle (stream, sample).
    DWORD flags;
};

*/

TS_AudioSampleSingle::TS_AudioSampleSingle(const char *f){
    sample = BASS_SampleLoad(0, f, 0, 0, 1, 0);
    channel = BASS_SampleGetChannel(sample, 0);
}

TS_AudioSampleSingle::~TS_AudioSampleSingle(void){

}

void TS_AudioSampleSingle::ResetPlayback(void){
    Stop();
    Play(false);
}

void TS_AudioSampleSingle::Play(bool repeat){
    if(IsPlaying()){
        Stop();
        Play(repeat);
        return;
    }
    BASS_SAMPLE sampleinfo;

    BASS_SampleGetInfo(sample, &sampleinfo);
    if(repeat)
        sampleinfo.flags|=BASS_SAMPLE_LOOP;
    else
        sampleinfo.flags&=(~BASS_SAMPLE_LOOP);

    BASS_SampleSetInfo(sample, &sampleinfo);

    channel = BASS_SampleGetChannel(sample, 0);
    BASS_ChannelPlay(channel, 1);
}

void TS_AudioSampleSingle::Stop(void){
    if(IsPlaying())
        BASS_ChannelStop(channel);
}

bool TS_AudioSampleSingle::IsPlaying(void) const{
    return (BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING)||(BASS_ChannelIsActive(channel) == BASS_ACTIVE_STALLED);
}


long long TS_AudioSampleSingle::GetPosition(void) const{
    if(IsPlaying()){
        long long pos = BASS_ChannelGetPosition(channel, BASS_POS_BYTE);
        double dlength = BASS_ChannelBytes2Seconds(channel, pos);
        return (long long)(dlength*DOUBLE_LITERAL(1000.0));
    }
    return 0;
}


long long TS_AudioSampleSingle::GetLength() const{
    long long end = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
    double dlength = BASS_ChannelBytes2Seconds(channel, end);
    return (long long)(dlength*DOUBLE_LITERAL(1000.0));
}

void TS_AudioSampleSingle::SetPosition(long long position){
    BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, (double)(position)/DOUBLE_LITERAL(1000.0)), BASS_POS_BYTE);
}

void TS_AudioSampleSingle::Pause(void){
    BASS_ChannelPause(channel);
}

#ifdef _WIN32
#undef fabs
#endif
