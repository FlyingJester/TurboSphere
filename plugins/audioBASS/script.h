#ifndef AUDIO_BASS_SCRIPT_HEAD
#define AUDIO_BASS_SCRIPT_HEAD

#include "frontend.h"
#include "bass_backend.h"

#ifdef _WIN32
#define __func__ __FUNCTION__
#endif

EXTERN_OBJECT_TEMPLATES(Sound);
EXTERN_OBJECT_TEMPLATES(SoundEffect);

v8Function SoundGetLength(V8ARGS);

v8Function LoadSound(V8ARGS);
v8Function LoadSoundEffect(V8ARGS);

v8Function PlaySound(V8ARGS);

v8Function PauseSound(V8ARGS);

v8Function StopSound(V8ARGS);

v8Function IsSoundPlaying(V8ARGS);

v8Function SoundSetVolume(V8ARGS);

#endif
