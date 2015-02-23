#pragma once
#include "cinnamon.hpp"
#include "sound.hpp"
#include "player.hpp"

namespace Cinnamon {

extern Turbo::JSPrototype<Sound> sound_proto;

void initScript(JSContext *ctx);
void closeScript(JSContext *ctx);

/* And it goes a little something like this...

// Create a Player (on init)

Player main_player;

InitPlayer(){
    main_player.init();
}

// Decide what format you are opening:
struct Cinnamon_Decoder *decoder = Cinnamon_CreateDecoder(Cinnamon_DefaultRate, Cinnamon_Stereo);

// Load the file into memory

// Decode the file:
void **decoded_buffer = nullptr;
unsigned long decoded_length = 0;
Cinnamon_Read(decoder, decoded_buffer, decoded_length, raw_buffer, raw_buffer_len);

// Create a Sound (in a JS constructor):
// Note that the last value is the same as the first to Cinnamon_CreateDecoder
Sound sound = main_player.load(decoded_buffer, decoded_length, Cinnamon_DefaultRate); 

// Destroy the decoder
Cinnamon_DestroyDecoder(decoder);

// Return the sound.

*/

bool SoundCtor(JSContext *ctx, unsigned argc, JS::Value *vp);
bool PlaySound(JSContext *ctx, unsigned argc, JS::Value *vp);
bool PauseSound(JSContext *ctx, unsigned argc, JS::Value *vp);
bool StopSound(JSContext *ctx, unsigned argc, JS::Value *vp);
bool RewindSound(JSContext *ctx, unsigned argc, JS::Value *vp);
bool SoundRepeatSetter(JSContext *ctx, unsigned argc, JS::Value *vp);
bool SoundRepeatGetter(JSContext *ctx, unsigned argc, JS::Value *vp);
void SoundFinalizer(JSFreeOp *fop, JSObject *obj);

}
