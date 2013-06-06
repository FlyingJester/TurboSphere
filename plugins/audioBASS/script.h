#ifndef AUDIO_BASS_SCRIPT_HEAD
#define AUDIO_BASS_SCRIPT_HEAD

#include "frontend.h"
#include "bass_backend.h"

EXTERN_OBJECT_TEMPLATES(Sound);
EXTERN_OBJECT_TEMPLATES(SoundEffect);

v8Function LoadSound(V8ARGS);
v8Function LoadSoundEffect(V8ARGS);

v8Function PlaySound(V8ARGS);
v8Function PlaySoundEffect(V8ARGS);

v8Function PauseSound(V8ARGS);
v8Function PauseSoundEffect(V8ARGS);

v8Function StopSound(V8ARGS);
v8Function StopSoundEffect(V8ARGS);


#endif
