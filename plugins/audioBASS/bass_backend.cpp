#include "include/bass.h"
#include "frontend.h"
#include "bass_backend.h"
#include <vector>
//TODO: Add seperate error for OS X.
#ifdef _WIN32
static const char *TS_MSG_DXError = "DirectX is not installed.\n";
#else
static const char *TS_MSG_DXError = "ALSA is not installed.";
#endif

static DWORD *defaultSoundFont;

static std::vector<TS_AudioChannel*> channels;

float fabs(float f){
    if(f<0.0f){
        return -f;
    }
    return f;
}

TS_AudioSample::~TS_AudioSample(){
    if(file!=NULL){
        free(file);
    }
}

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
        printf("[" PLUGINNAME "] Info: Initialized BASSmidi.\n");
    }
    defaultSoundFont = (DWORD *)malloc(sizeof(DWORD));

    *defaultSoundFont = BASS_MIDI_FontInit(string(TS_Dirs->system).append(TS_Conf->systemsoundfont).c_str(), 0);

/*
    BOOL defaultFontLoaded = BASS_MIDI_FontLoad(*defaultSoundFont, -1, -1);


*/
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


/*
typedef struct {
	HSOUNDFONT font;	// soundfont
	int spreset;		// source preset number
	int sbank;			// source bank number
	int dpreset;		// destination preset/program number
	int dbank;			// destination bank number
	int dbanklsb;		// destination bank number LSB
} BASS_MIDI_FONTEX;
*/
    return 0;
}

int TS_InitBass(){

    DWORD version = BASS_GetVersion();

    if(HIWORD(version)!=BASSVERSION){
        printf("[" PLUGINNAME "] Warning: Wrong version of BASS loaded (expected %x, version was %x). Crashes will likely follow.\n", BASSVERSION, version);
    }

	if (!BASS_Init(-1,44100,0,NULL,NULL)) {
	    int error = BASS_ErrorGetCode();
        TS_ExplainBassErrorCode(error, true, false);

        BOOL couldstart = 1;

        if((error==BASS_ERROR_ALREADY)||(error==BASS_ERROR_ALREADY)){
            BASS_Free();
            couldstart = BASS_Init(-1, 44100, 0, NULL, NULL);
            error = BASS_ErrorGetCode();
            if(error==BASS_ERROR_FORMAT){
                BASS_Free();
            }
        }

        if(error==BASS_ERROR_FORMAT){
            BASS_Init(-1, 22050, FAILSAFE_BASS_INIT_FLAGS, NULL, NULL);
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
    TS_InitBassMidi();
    BASS_Start();
    return 0;
}

void TS_RestartChannel(TS_AudioChannel *channel){
    BASS_ChannelSetPosition(channel->channel, 0, BASS_POS_BYTE);
    BASS_ChannelPlay(channel->channel, channel->repeat);
}

void TS_PlaySound(TS_AudioSample *sample, bool loop){
    for(size_t i = 0; i<channels.size(); i++){
        if(channels[i]->sample->handle == sample->handle){
            channels[i]->repeat = (loop)?1:0;
            TS_RestartChannel(channels[i]);
            return;
        }
    }
    TS_AudioChannel *channel = TS_CreateChannel(sample);
    channel->repeat =(loop)?1:0;
    channel->Play(channel->repeat);
}

void TS_PlaySoundEffect(TS_AudioSample *sample){
    TS_AudioChannel *channel = TS_CreateChannel(sample);
    channel->Play(0);
}

TS_AudioSample::TS_AudioSample(){
    handle   = 0;
    file     = (char*)malloc(1);
    *file    = '\0';
    playbacks= 0;
    volume   = 1.0f;
}

TS_AudioChannel::TS_AudioChannel(){
    channel = 0;
    repeat  = false;
}

TS_AudioChannel::TS_AudioChannel(TS_AudioSample *_sample){
    sample = _sample;
    if(_sample->type==TS_STREAM)
        channel = _sample->handle;

    repeat  = false;
}

TS_AudioChannel *TS_CreateChannel(TS_AudioSample *sample){
    TS_AudioChannel *channel = new TS_AudioChannel(sample);
    channels.push_back(channel);
    return channel;
}

void TS_AudioChannel::Play(int loops){

    if((sample->type==TS_STREAM)&&(channel==sample->handle)){
        BASS_ChannelPlay(channel, loops);
        return;
    }

    if(sample->type==TS_STREAM){
        channel = sample->handle;
    }
    else if(sample->type==TS_SAMPLE){
        channel = BASS_SampleGetChannel(sample->handle, 0);
        int error = BASS_ErrorGetCode();
        if(error)
            TS_ExplainBassErrorCode(error, false, false);
    }


/*
typedef struct {
	HSOUNDFONT font;	// soundfont
	int spreset;		// source preset number
	int sbank;			// source bank number
	int dpreset;		// destination preset/program number
	int dbank;			// destination bank number
	int dbanklsb;		// destination bank number LSB
} BASS_MIDI_FONTEX;


    BASS_MIDI_FONTEX fontex;

    BASS_MIDI_StreamGetFonts(channel, &fontex, 1);

    fontex.font = *defaultSoundFont;

    BASS_MIDI_StreamSetFonts(channel, &fontex, 1);
*/
    BASS_ChannelPlay(channel, 0);
    if(sample->type==TS_STREAM){
        BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, 1.0f);
    }

}

/*
class TS_AudioSample{
public:
    TS_AudioSample(void);
    ~TS_AudioSample(void);

    HSAMPLE sample;   //BASS-side sample.
    char *file; //Filename.
    int playbacks;    //Number of simultaneous playbacks.
    DWORD flags;
    float volume;
};
*/

TS_AudioSample::TS_AudioSample(const char *filename){
    //BASS_SampleLoad(           BOOL mem, void *file, QWORD offset, DWORD length, DWORD max, DWORD flags);
    //BASS_MIDI_StreamCreateFile(BOOL mem, void *file, QWORD offset, QWORD length, DWORD flags, DWORD freq);
    //BASS_StreamCreateFile(     BOOL mem, void *file, QWORD offset, QWORD length, DWORD flags);
    file = STRDUP(filename);

    type = TS_STREAM;

    handle = 0;
    if(type==TS_STREAM){
        //handle = BASS_StreamCreateFile(0, filename, 0, 0, 0);
        handle = BASS_MIDI_StreamCreateFile(0, filename, 0, 0, BASS_SAMPLE_FLOAT|BASS_SAMPLE_LOOP|BASS_MIDI_DECAYSEEK, 1);
        if (!handle){
            handle = BASS_StreamCreateFile(0, filename, 0, 0, 0);
        }
        int error = BASS_ErrorGetCode();
        if((!handle)||(error)){
            printf("[" PLUGINNAME "] TS_AudioSample Error: Could not load stream %s.", filename);
            if(error)
                printf(TS_MSG_UnkownErrorPRT, error);

            printf("\n");
        }
    }
    else if(type==TS_SAMPLE){

        handle = BASS_SampleLoad(0, filename, 0, 0, 8, 0);
        BASS_SAMPLE sampleInfo;
        BASS_SampleGetInfo(handle, &sampleInfo);
        sampleInfo.volume = 1.0f;
        sampleInfo.pan    = 0.0f;
        BASS_SampleSetInfo(handle, &sampleInfo);
        int error = BASS_ErrorGetCode();
        TS_ExplainBassErrorCode(error, false, false);

        if(!handle){
            printf("[" PLUGINNAME "] TS_AudioSample Error: Could not load sample %s.\n", filename);
        }
    }
}

void TS_AudioSample::SetVolume(float vol){
    if(type!=TS_SAMPLE)
        return; //TODO: Add volume support for streams.

    if(volume==vol){
        return;
    }

    if((fabs(volume-vol)<VOLUME_RESOLUTION)&&(vol!=0.0f)&&(vol!=1.0f)){
        return;
    }

    volume = vol;

    BASS_SAMPLE sampleInfo;
    BASS_SampleGetInfo(handle, &sampleInfo);
    sampleInfo.volume = vol;
    BASS_SampleSetInfo(handle, &sampleInfo);
}

void TS_AudioSample::SetVolume(unsigned char vol){
    if(type!=TS_SAMPLE)
        return; //TODO: Add volume support for streams.

    float nvol = ((float)vol);
    nvol/=255.0f;
    this->SetVolume(nvol);
}

float TS_AudioSample::GetVolume(){
    if(type!=TS_SAMPLE)
        return 0.1f; //TODO: Add volume support for streams.

    BASS_SAMPLE sampleInfo;
    BASS_SampleGetInfo(handle, &sampleInfo);
    volume = sampleInfo.volume;
    return volume;
}

void TS_AudioSample::SetPan(float p){
    if(type!=TS_SAMPLE)
        return; //TODO: Add volume support for streams.

    if(pan==p){
        return;
    }

    if((fabs(pan-p)<PAN_RESOLUTION)&&(p!=0.0f)&&(p!=1.0f)&&(p!=-1.0f)){
        return;
    }

    pan = p;

    BASS_SAMPLE sampleInfo;
    BASS_SampleGetInfo(handle, &sampleInfo);
    sampleInfo.pan = p;
    BASS_SampleSetInfo(handle, &sampleInfo);
}

void TS_AudioSample::SetPan(unsigned char p){
    if(type!=TS_SAMPLE)
        return; //TODO: Add volume support for streams.
    float np = ((float)p);
    np/=255.0f;
    this->SetPan(np);
}

float TS_AudioSample::GetPan(){
    if(type!=TS_SAMPLE)
        return 0.0f; //TODO: Add volume support for streams.
    BASS_SAMPLE sampleInfo;
    BASS_SampleGetInfo(handle, &sampleInfo);
    volume = sampleInfo.pan;
    return pan;
}

void TS_MakeSampleSound(TS_AudioSample *sample){
    BASS_SAMPLE sampleInfo;
    BASS_SampleGetInfo(sample->handle, &sampleInfo);
    sampleInfo.priority = 0xFFFF;
    BASS_SampleSetInfo(sample->handle, &sampleInfo);

}

void TS_MakeSampleSoundEffect(TS_AudioSample *sample){
    BASS_SAMPLE sampleInfo;
    BASS_SampleGetInfo(sample->handle, &sampleInfo);
    sampleInfo.priority = 0xFFFF;
    BASS_SampleSetInfo(sample->handle, &sampleInfo);
}

void TS_SetGlobalVolume(float volume){
    BASS_SetVolume(volume);
}

void TS_ResetGlobalVolume(){
    BASS_SetVolume(1.0f);
}
