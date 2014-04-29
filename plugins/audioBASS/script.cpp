#include "script.h"

//DECLARE_OBJECT_TEMPLATES(Sound);
//DECLARE_OBJECT_TEMPLATES(SoundEffect);

void TS_SoundFinalizer(const v8::WeakCallbackData<v8::Object, TS_Sound> &args) {
    delete args.GetParameter();
    args.GetValue().Clear();
}

void InitSound(void){
    SoundObject.Finalize = TS_SoundFinalizer;
}

void InitSoundEffect(void){
    SoundEffectObject.Finalize = TS_SoundFinalizer;
}


Turbo::JSFunction LoadSound(Turbo::JSArguments args){

    const int sig[] = {Turbo::String, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
        return;
/*
    if(args.Length()<1){
        THROWERROR(" Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE
*/
    TS_Sound *sound = NULL;
	TS_Directories *TS_dirs = GetDirs();
        v8::String::Utf8Value str(args[0]);
        const char *soundname = *str;

        //THROWERROR(string(" Error: Could not load Sound ").append(soundname).c_str());
    bool stream = true;
    if(args.Length()==2){
        Turbo::CheckArg::Bool(args, 1, __func__);
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
    if(!sound){
        Turbo::SetError(args, string("Error: Could not load Sound ").append(soundname).c_str());
        return;
    }

    //auto ret =
    Turbo::WrapObject(args, SoundObject, sound);

    //args.GetReturnValue().Set(ret);
//    args.GetReturnValue().Set(Turbo::WrapObject(SoundObject,sound));
    //return;
}

Turbo::JSFunction LoadSoundEffect(Turbo::JSArguments args){
    /*
    if(args.Length()<1){
        THROWERROR("LoadSoundEffect Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);
    */
    int sig[] = {Turbo::String, Turbo::Int, 0};
    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
        return;

    TS_AudioSimul sim = TS_MULTIPLE;

    if(Turbo::CheckArg::CheckSig(args, 2, sig, false)){

        int s = args[1]->Int32Value();
        if((s!=TS_SINGLE)&&(s!=TS_MULTIPLE)){
            Turbo::SetError(args, string("[" PLUGINNAME "] ").append(__func__).append(" Error: Argument 1 must be SE_SINGLE or SE_MULTIPLE.").c_str(), v8::Exception::RangeError);
            return;
        }
        sim = (TS_AudioSimul)s;
    }

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
    if(!sound){
        Turbo::SetError(args, string("Error: Could not load SoundEffect ").append(soundname).c_str());
        return;
    }


    Turbo::WrapObject(args, SoundEffectObject, sound);
//    args.GetReturnValue().Set((const v8::Persistent<v8::Object> &)(Turbo::WrapObject(SoundEffectObject, sound)));
//    return;
}

Turbo::JSFunction SoundGetLength(Turbo::JSArguments args){
    args.GetReturnValue().Set(
        v8::Integer::New(
            v8::Isolate::GetCurrent(),
            Turbo::GetMemberSelf<TS_Sound>(args)->GetLength()
        )
    );
}

Turbo::JSFunction SoundSetVolume(Turbo::JSArguments args){

//    if(args.Length()<1){
//        THROWERROR(" Error: Called with no arguments.");
//    }

//    CHECK_ARG_INT(0);

    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
        return;

    int leveli = args[0]->IntegerValue();

    if(leveli<0)
        leveli=0;
    else if(leveli>255)
        leveli=255;

    unsigned char level = leveli;

    Turbo::GetMemberSelf<TS_Sound>(args)->SetVolume(level);
}

Turbo::JSFunction PlaySound(Turbo::JSArguments args){
    bool loop = false;

    int sig[] = {Turbo::Bool, 0};

    if(Turbo::CheckArg::CheckSig(args, 1, sig, false)){
        loop = args[0]->BooleanValue();
    }

   Turbo::GetMemberSelf<TS_Sound>(args)->Play(loop);
}

Turbo::JSFunction SoundIsSeekable(Turbo::JSArguments args){

}

Turbo::JSFunction SoundSeek(Turbo::JSArguments args){

}

Turbo::JSFunction SoundGetPosition(Turbo::JSArguments args){
    args.GetReturnValue().Set( v8::Integer::New(v8::Isolate::GetCurrent(), Turbo::GetMemberSelf<TS_Sound>(args)->GetPosition()));
}

Turbo::JSFunction PauseSound(Turbo::JSArguments args){
    Turbo::GetMemberSelf<TS_Sound>(args)->Pause();
}

Turbo::JSFunction StopSound(Turbo::JSArguments args){
    Turbo::GetMemberSelf<TS_Sound>(args)->Stop();
}

Turbo::JSFunction IsSoundPlaying(Turbo::JSArguments args){
    args.GetReturnValue().Set( v8::Boolean::New(v8::Isolate::GetCurrent(), Turbo::GetMemberSelf<TS_Sound>(args)->IsPlaying()));
}

