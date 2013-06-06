#include "frontend.h"
#include "bass_backend.h"
#include "script.h"

v8Function PlaySound(V8ARGS);
v8Function PlaySoundEffect(V8ARGS);

v8Function PauseSound(V8ARGS);
v8Function PauseSoundEffect(V8ARGS);

v8Function StopSound(V8ARGS);
v8Function StopSoundEffect(V8ARGS);

void* LoadSoundPointer       = V8FUNCPOINTER(LoadSound);
void* LoadSoundEffectPointer = V8FUNCPOINTER(LoadSoundEffect);

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

initFunction Init(){
    TS_InitBass();

    INIT_OBJECT_TEMPLATES(Sound);
    SET_CLASS_NAME(Sound, "Sound");

    INIT_OBJECT_TEMPLATES(SoundEffect);
    SET_CLASS_NAME(SoundEffect, "SoundEffect");


    ADD_TO_PROTO(Sound,       "Play",  PlaySound);
    ADD_TO_PROTO(SoundEffect, "Play",  PlaySoundEffect);

    ADD_TO_PROTO(Sound,       "Pause", PauseSound);
    ADD_TO_PROTO(SoundEffect, "Pause", PauseSoundEffect);

    ADD_TO_PROTO(Sound,       "Stop",  StopSound);
    ADD_TO_PROTO(SoundEffect, "Stop",  StopSoundEffect);

    return (initFunction)PLUGINNAME;
}

void Close(){
    BASS_Pause();
    BASS_Stop();
    BASS_PluginFree(0);
    BASS_Free();
}


int GetNumFunctions(){
    return NUMFUNCS;
}

functionArray GetFunctions(){
    numerate(true);

    functionArray funcs = (functionArray)calloc(NUMFUNCS, sizeof(void*));

    funcs[numerate(false)] = LoadSoundPointer;
    funcs[numerate(false)] = LoadSoundEffectPointer;

    return funcs;
}

nameArray GetFunctionNames(){
    numerate(true);

    nameArray funcnames = (nameArray)calloc(NUMFUNCS, sizeof(functionName));

    funcnames[numerate(false)] = (functionName)"Sound";
    funcnames[numerate(false)] = (functionName)"SoundEffect";

    return funcnames;
}

int GetNumVariables(){
    return NUMVARS;
}

v8FunctionArray GetVariables(){
    return NULL;
}

nameArray GetVariableNames(){
    return NULL;
}
