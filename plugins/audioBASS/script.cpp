#include "script.h"

DECLARE_OBJECT_TEMPLATES(Sound);
DECLARE_OBJECT_TEMPLATES(SoundEffect);

void TS_SoundFinalizer(V8FINALIZERARGS) {
    TS_Sound* sound = (TS_Sound*)parameter;
    delete sound;
    object->Dispose();
}

void TS_SoundEffectFinalizer(V8FINALIZERARGS) {
    TS_Sound* sound = (TS_Sound*)parameter;
    delete sound;
    object->Dispose();
}

v8Function LoadSound(V8ARGS){
    if(args.Length()<1){
        THROWERROR(" Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE

    TS_Sound *sound = NULL;
	TS_Directories *TS_dirs = GetDirs();
        v8::String::Utf8Value str(args[0]);
        const char *soundname = *str;

        //THROWERROR(string(" Error: Could not load Sound ").append(soundname).c_str());
    bool stream = true;
    if(args.Length()==2){
        CHECK_ARG_BOOL(1);
        stream = args[1]->BooleanValue();
    }

    if(stream){
        printf("Opened a file as a stream.\n");
        sound = new TS_AudioStream(string(TS_dirs->sound).append(soundname).c_str());
    }
    else{
        printf("Opened a file as a sample.\n");
        sound = new TS_AudioSampleSingle(string(TS_dirs->sound).append(soundname).c_str());
    }
    if(!sound)
        THROWERROR(string("Error: Could not load Sound ").append(soundname).c_str());

    END_OBJECT_WRAP_CODE(Sound, sound);
    return v8::Undefined();
}

v8Function LoadSoundEffect(V8ARGS){
    if(args.Length()<1){
        THROWERROR("LoadSoundEffect Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    TS_AudioSimul sim = TS_MULTIPLE;

    if(args.Length()>1){
        CHECK_ARG_INT(1);
        int s = args[1]->Int32Value();
        if((s!=TS_SINGLE)&&(s!=TS_MULTIPLE)){
            THROWERROR(" Error: Argument 1 must be SE_SINGLE or SE_MULTIPLE.");
        }
        sim = (TS_AudioSimul)s;
    }

    BEGIN_OBJECT_WRAP_CODE

    TS_Sound *sound = NULL;
	TS_Directories *TS_dirs = GetDirs();
    v8::String::Utf8Value str(args[0]);
    const char *soundname = *str;

    if(sim==TS_SINGLE){
        sound = new TS_AudioSampleSingle(  string(TS_dirs->sound).append(soundname).c_str());
    }
    else{
        sound = new TS_AudioSampleMultiple(string(TS_dirs->sound).append(soundname).c_str());
    }
    if(!sound)
        THROWERROR(string("Error: Could not load SoundEffect ").append(soundname).c_str());


    END_OBJECT_WRAP_CODE(SoundEffect, sound);
    return v8::Undefined();
}

v8Function SoundGetLength(V8ARGS){
    return v8::Integer::New(GET_SELF(TS_Sound*)->GetLength());
}

v8Function SoundSetVolume(V8ARGS){

    if(args.Length()<1){
        THROWERROR(" Error: Called with no arguments.");
    }

    CHECK_ARG_INT(0);

    int leveli = args[0]->IntegerValue();

    if(leveli<0)
        leveli=0;
    else if(leveli>255)
        leveli=255;

    unsigned char level = leveli;

    GET_SELF(TS_Sound*)->SetVolume(level);
    return v8::Undefined();
}

v8Function PlaySound(V8ARGS){
    bool loop = false;
    if(args.Length()>0){
        CHECK_ARG_BOOL(0);
        loop = args[0]->BooleanValue();
    }

    GET_SELF(TS_Sound*)->Play(loop);
    return v8::Undefined();
}

v8Function SoundIsSeekable(V8ARGS){
    return v8::False();
}

v8Function SoundSeek(V8ARGS){
    return v8::Undefined();
}

v8Function SoundGetPosition(V8ARGS){
    return v8::Integer::New(GET_SELF(TS_Sound*)->GetPosition());
}

v8Function PauseSound(V8ARGS){
    GET_SELF(TS_Sound*)->Pause();
    return v8::Undefined();
}

v8Function StopSound(V8ARGS){
    GET_SELF(TS_Sound*)->Stop();
    return v8::Undefined();
}

v8Function IsSoundPlaying(V8ARGS){
    return v8::Boolean::New(GET_SELF(TS_Sound*)->IsPlaying());
}

