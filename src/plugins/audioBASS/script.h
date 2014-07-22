#ifndef AUDIO_BASS_SCRIPT_HEAD
#define AUDIO_BASS_SCRIPT_HEAD

#include "frontend.h"
#include "bass_backend.h"

#ifdef _WIN32
#define __func__ __FUNCTION__
#endif

extern Turbo::JSObj<TS_Sound>       SoundObject;
extern Turbo::JSObj<TS_Sound> SoundEffectObject;

//EXTERN_OBJECT_TEMPLATES(Sound);
//EXTERN_OBJECT_TEMPLATES(SoundEffect);

Turbo::JSFunction SoundGetLength(Turbo::JSArguments);

Turbo::JSFunction LoadSound(Turbo::JSArguments);
Turbo::JSFunction LoadSoundEffect(Turbo::JSArguments);

Turbo::JSFunction PlaySound(Turbo::JSArguments);

Turbo::JSFunction PauseSound(Turbo::JSArguments);

Turbo::JSFunction StopSound(Turbo::JSArguments);

Turbo::JSFunction IsSoundPlaying(Turbo::JSArguments);

Turbo::JSFunction SoundSetVolume(Turbo::JSArguments);

#endif
