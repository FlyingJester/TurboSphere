#include "frontend.h"
#include "bass_backend.h"
#include "script.h"

Turbo::JSObj<TS_Sound> SoundObject       = Turbo::JSObj<TS_Sound>();
Turbo::JSObj<TS_Sound> SoundEffectObject = Turbo::JSObj<TS_Sound>();

Turbo::JSFunction PlaySound(Turbo::JSArguments);
Turbo::JSFunction PlaySoundEffect(Turbo::JSArguments);

Turbo::JSFunction PauseSound(Turbo::JSArguments);
Turbo::JSFunction PauseSoundEffect(Turbo::JSArguments);

Turbo::JSFunction StopSound(Turbo::JSArguments);
Turbo::JSFunction StopSoundEffect(Turbo::JSArguments);

void* LoadSoundPointer       = (void *)((Turbo::JSFunction (*)(Turbo::JSArguments))(LoadSound));
void* LoadSoundEffectPointer = (void *)((Turbo::JSFunction (*)(Turbo::JSArguments))(LoadSoundEffect));

/*
void* PlaySoundPointer       = V8FUNCPOINTER(PlaySound);
void* PlaySoundEffectPointer = V8FUNCPOINTER(PlaySoundEffect);

void* PauseSoundPointer      = V8FUNCPOINTER(PauseSound);
void* PauseSoundEffectPointer= V8FUNCPOINTER(PauseSoundEffect);

void* StopSoundPointer       = V8FUNCPOINTER(StopSound);
void* StopSoundEffectPointer = V8FUNCPOINTER(StopSoundEffect);
*/

int numerate(bool reset){
    static int i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return i-1;
}

const char * Init(){
    TS_InitBass();

    SoundObject.SetTypeName("Sound");
    SoundEffectObject.SetTypeName("SoundEffect");

    SoundObject.AddToProto(         "play", PlaySound);
    SoundEffectObject.AddToProto(   "play", PlaySound);

    SoundObject.AddToProto(         "pause", PauseSound);
    SoundEffectObject.AddToProto(   "pause", PauseSound);

    SoundObject.AddToProto(         "stop", StopSound);
    SoundEffectObject.AddToProto(   "stop", StopSound);

    SoundObject.AddToProto(         "isPlaying", IsSoundPlaying);
    SoundEffectObject.AddToProto(   "isPlaying", IsSoundPlaying);

    SoundObject.AddToProto(         "setVolume", SoundSetVolume);
    SoundEffectObject.AddToProto(   "setVolume", SoundSetVolume);

    SoundObject.AddToProto(         "getLength", SoundGetLength);

    return (const char *)PLUGINNAME;
}

void Close(){
    //BASS_Pause();
    BASS_Stop();
    BASS_PluginFree(0);
    BASS_Free();
}


int GetNumFunctions(){
    return NUMFUNCS;
}

Turbo::JSFunctionArray GetFunctions(){
    numerate(true);

    static Turbo::JSFunctionArray funcs = (Turbo::JSFunctionArray)calloc(NUMFUNCS, sizeof(void*));

    funcs[numerate(false)] = LoadSound;
    funcs[numerate(false)] = LoadSoundEffect;

    return funcs;
}

Turbo::JSNameArray GetFunctionNames(){
    numerate(true);

    static Turbo::JSNameArray funcnames = (Turbo::JSNameArray)calloc(NUMFUNCS, sizeof(Turbo::JSFunctionName));

    funcnames[numerate(false)] = (Turbo::JSFunctionName)"Sound";
    funcnames[numerate(false)] = (Turbo::JSFunctionName)"SoundEffect";

    return funcnames;
}

int GetNumVariables(){
    return NUMVARS;
}

Turbo::JSValueArray GetVariables(){
    numerate(true);

    auto iso = v8::Isolate::GetCurrent();

    static Turbo::JSValueArray vars = (Turbo::JSValueArray)calloc(NUMVARS, sizeof(Turbo::JSValue));

    vars[numerate(false)]=v8::Integer::New(iso, TS_SINGLE);
    vars[numerate(false)]=v8::Integer::New(iso, TS_MULTIPLE);

    return vars;
}

Turbo::JSNameArray GetVariableNames(){
    numerate(true);

    static Turbo::JSNameArray varnames = (Turbo::JSNameArray)calloc(NUMFUNCS, sizeof(Turbo::JSVariableName));

    varnames[numerate(false)] = (Turbo::JSVariableName)"SE_SINGLE";
    varnames[numerate(false)] = (Turbo::JSVariableName)"SE_MULTIPLE";

    return varnames;
}
