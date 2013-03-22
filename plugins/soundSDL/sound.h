#ifndef SOUND_HEAD
#define SOUND_HEAD

#include "SDL_V8.h"
#include <vector>

#define MAX_CHANNELS 32

#define SOUND_OBJ       0x0700
#define SNDFX_OBJ       0x0007
#define YM2612_OBJ      0x1017
#define UNIVERSAL_CHANS 0x0619

#define NUM_SOUNDS  8
#define NUM_SNDFXS  16
#define NUM_UNICHN  6
#define NUM_YM2612  2

#define SOUND_GROUP     1
#define SNDFX_GROUP     2
#define YM2612_GROUP    3
#define UNIVS_GROUP     4

void SoundInit();
void SoundClose();

class TS_Sound{
public:
	TS_Sound(const char*, bool);
	TS_Sound(SDL_RWops*, bool);
	TS_Sound();
	~TS_Sound();

	int play(int);

    int type;
    bool isPlaying;
    int channel;
	Mix_Chunk *sound;
	Uint8 volume;
	Uint8 pan;
};

v8::Handle<v8::Value> LoadSound(const v8::Arguments& args);

v8::Handle<v8::Value> TS_playSound(const v8::Arguments& args);
v8::Handle<v8::Value> TS_isSoundPlaying(const v8::Arguments& args);

#endif
