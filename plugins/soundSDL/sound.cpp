#include "sound.h"
#include "configmanager/opengame.h"

using namespace std;

v8::Persistent<v8::FunctionTemplate> Soundtempl;
v8::Persistent<v8::ObjectTemplate> SoundInsttempl;
v8::Persistent<v8::ObjectTemplate> Soundproto;

int numChannels;

std::vector <TS_Sound*> channelMap(MAX_CHANNELS);


void TS_SoundFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    TS_Sound* sound = (TS_Sound*)parameter;
    delete sound;
    object.Dispose();
    object.Clear();
}

void EndChannel(int channel){
    if(channelMap[channel]==NULL){
        printf("TS Audio Error: empty channel halted.\n");
    }
    channelMap[channel]->isPlaying  = false;
    channelMap[channel]->channel    = -1;
    channelMap[channel]             = NULL;
}

void SoundInit(){
    numChannels = TS_conf.soundchannels;

    Mix_AllocateChannels(TS_conf.soundchannels);

    for(int i = 0; i<TS_conf.soundchannels; i++){
        channelMap[i] = NULL;
    }

    Mix_GroupChannels(0, 7,     SOUND_GROUP);
    Mix_GroupChannels(8, 23,    SNDFX_GROUP);
    Mix_GroupChannels(24,25,    YM2612_GROUP);

    Mix_ChannelFinished(EndChannel);
    Soundtempl = *v8::FunctionTemplate::New();
	SoundInsttempl = *Soundtempl->InstanceTemplate();
    Soundproto     = *Soundtempl->PrototypeTemplate();
	Soundproto->Set("play", v8::FunctionTemplate::New(TS_playSound));
	Soundproto->Set("isPlaying", v8::FunctionTemplate::New(TS_isSoundPlaying));

}

void SoundClose(){
    Soundtempl.Dispose();
	SoundInsttempl.Dispose();
}

int GetChannel(int type){

    int chan = Mix_GroupAvailable(type);

    while(chan<0){
        channelMap.resize(channelMap.size()+1);
        Mix_AllocateChannels(channelMap.size());
        Mix_GroupChannel(channelMap.size()-1, type);
        chan = Mix_GroupAvailable(type);
    }

    return chan;

}

v8::Handle<v8::Value> LoadSound(const v8::Arguments& args) {
  v8::HandleScope loadsoundscope;
   v8::Handle<v8::Value> external;
  //v8::Handle<v8::Object> wrapper;

    if(args.Length()<1){
        return v8::ThrowException(v8::String::New("LoadSound Error: Called with no arguments."));
    }

    TS_Sound *sound = NULL;

    if (args[0]->IsExternal()) {
        external = v8::Local<v8::External>::Cast(args[0]);
    } else {

        CHECK_ARG_STR(0, "LoadSound Error: Could not parse arg 0 to string!");

        v8::String::AsciiValue str(args[0]);
        const char *file = *str;

        SDL_RWops *soundfile = SDL_RWFromFile(string(TS_dirs.sound).append(file).c_str(), "rb");

        if(soundfile==NULL){
            printf("LoadSound error: %s\n", SDL_GetError());

            return v8::ThrowException(v8::String::New(string("Could not open sound ").append(string(TS_dirs.sound).append(file).c_str()).c_str()));
        }

        sound = new TS_Sound(soundfile, false);

        external = v8::External::New(sound);
    }
  	SoundInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> Soundctor = Soundtempl->GetFunction();

	v8::Local<v8::Object> Soundobj = Soundctor->NewInstance();

    v8::Persistent<v8::Object> PSoundobj = v8::Persistent<v8::Object>::New(Soundobj);
    Soundobj.Clear();
    Soundctor.Clear();

    PSoundobj.MakeWeak(sound, TS_SoundFinalizer);
	PSoundobj->SetInternalField(0, external);
    external.Clear();

    return loadsoundscope.Close(PSoundobj);
}

v8::Handle<v8::Value> TS_playSound(const v8::Arguments& args){
    bool loop = false;
    if(args.Length()>0){
        CHECK_ARG_BOOL(0, "TS_playSound Error: arg 0 is not a boolean.");
        loop = args[0]->BooleanValue();
    }

    v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	//static_cast<SDL_Surface*>(ptr);
	if(static_cast<TS_Sound*>(ptr)->play(0)<0){
        return v8::ThrowException(v8::String::New("TS_Sound play Error: could not play."));
	}
	return v8::Undefined();

}

v8::Handle<v8::Value> TS_isSoundPlaying(const v8::Arguments& args){

    v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	//static_cast<SDL_Surface*>(ptr);
	return v8::Boolean::New(static_cast<TS_Sound*>(ptr)->isPlaying);
}

int TS_Sound::play(int loops){
    int chan         = GetChannel(type);
    channel          = chan;
    Mix_Volume(chan, 64);
    channelMap[chan] = this;
    isPlaying        = true;
    if(Mix_PlayChannel(chan, sound, loops)==-1){
        printf("TS_Sound play Error: %s\n", Mix_GetError());
        return -1;
    }
    return 1;
}

TS_Sound::TS_Sound(const char *file, bool stream){
    sound = Mix_LoadWAV(file);  //This function has a bad name. It'll load any SDL_Mixer-compatible file.
	volume  = 127;
	pan     = 127;
	type    = SOUND_GROUP;
}

TS_Sound::TS_Sound(SDL_RWops *file, bool stream){
    sound = Mix_LoadWAV_RW(file, 0);
	if(sound==NULL){
	printf("Sound from RW was null. Error: %s\n", Mix_GetError());
	}
	else{
	printf("Sound from RW was good. ");
	}
	if(file==NULL){
	printf("RW passed was null.\n");
	}
	else{
	printf("RW passed to sound constructor was good.\n");
	}
	volume  = 127;
	pan     = 127;
}



TS_Sound::~TS_Sound(){

    if(isPlaying&&channel>=0){
        Mix_HaltChannel(channel);
    }
    Mix_FreeChunk(sound);
}
