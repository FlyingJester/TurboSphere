#include "script.h"

DECLARE_OBJECT_TEMPLATES(Sound);
DECLARE_OBJECT_TEMPLATES(SoundEffect);

void TS_SoundFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    TS_AudioSample* sample = (TS_AudioSample*)parameter;
    delete sample;
    object.Dispose();
}

void TS_SoundEffectFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    TS_AudioSample* sample = (TS_AudioSample*)parameter;
    delete sample;
    object.Dispose();
}

v8Function LoadSound(V8ARGS){
    if(args.Length()<1){
        THROWERROR(" Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE

    TS_AudioSample *sound = NULL;
	TS_Directories *TS_dirs = GetDirs();
        v8::String::Utf8Value str(args[0]);
        const char *soundname = *str;

        //THROWERROR(string(" Error: Could not load Sound ").append(soundname).c_str());

        sound = new TS_AudioSample(string(TS_dirs->sound).append(soundname).c_str());

    END_OBJECT_WRAP_CODE(Sound, sound);
    return v8::Undefined();
}

v8Function LoadSoundEffect(V8ARGS){
    if(args.Length()<1){
        THROWERROR("LoadSoundEffect Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE

    TS_AudioSample *sound = NULL;
	TS_Directories *TS_dirs = GetDirs();
        v8::String::Utf8Value str(args[0]);
        const char *soundname = *str;

        THROWERROR(string("LoadSoundEffect Error: Could not load SoundEffect ").append(soundname).c_str());

        sound = new TS_AudioSample(string(TS_dirs->sound).append(soundname).c_str());

    END_OBJECT_WRAP_CODE(SoundEffect, sound);
    return v8::Undefined();
}

v8Function PlaySound(V8ARGS){
    bool loop = false;
    if(args.Length()>0){
        CHECK_ARG_BOOL(0);
        loop = args[0]->BooleanValue();
    }

    TS_PlaySound(GET_SELF(TS_AudioSample*), loop);
    return v8::Undefined();
}

v8Function PlaySoundEffect(V8ARGS){
    TS_PlaySoundEffect(GET_SELF(TS_AudioSample*));
    return v8::Undefined();
}

v8Function PauseSound(V8ARGS){

    return v8::Undefined();
}

v8Function PauseSoundEffect(V8ARGS){

    return v8::Undefined();
}

v8Function StopSound(V8ARGS){

    return v8::Undefined();
}

v8Function StopSoundEffect(V8ARGS){

    return v8::Undefined();
}
