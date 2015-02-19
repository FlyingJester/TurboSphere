#include "player.hpp"

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alext.h>
#include <AL/alc.h>
#endif
#include <cassert>

namespace Cinnamon{

void Player::makeCurrent() const{
    assert(context);
    alcMakeContextCurrent(static_cast<ALCcontext *>(context));
}

Player::Player()
  : dev(nullptr)
  , context(nullptr)
  , format(AL_FORMAT_STEREO16){
    
}

bool Player::init(){
    dev = alcOpenDevice(nullptr);
    if(!dev)
        return false;

    context = alcCreateContext(static_cast<ALCdevice *>(dev), nullptr);
    if(!context)
        return false;

    makeCurrent();

    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_ORIENTATION, 0.0f, 0.0f, 0.0f);
    
    return true;
    
}

Sound Player::load(const int16_t *from, size_t num, int num_channels, int samples_per_second, size_t full_length){
    
    makeCurrent();
    
    Sound sound(*this);
    sound.num_channels = num_channels;
    sound.samples_per_second = samples_per_second;

    sound.length = static_cast<float>((full_length==0)?num:full_length)/samples_per_second;  
    
    switch(num_channels){
        case 1:
            sound.format = AL_FORMAT_MONO16;
            break;
        case 2:
            sound.format = AL_FORMAT_STEREO16;
            break;
        default:
            sound.format = format;
    }
    
    addToSound(sound, from, num);
    
    sound.gain = 1.0f;
    
    return sound;
}

void Player::addToSound(Sound *sound, const int16_t *from, size_t num){

    makeCurrent();

    if(buffers.empty())
        sound->returnBuffers();

    if(buffers.empty()){
        const unsigned num_new_buffers = 8;
        
        ALuint new_buffers[num_new_buffers];
        alGenBuffers(num_new_buffers, new_buffers);
        
        for(unsigned i = 0; i<num_new_buffers; i++)
            buffers.push_back(new_buffers[i]);
    }

    assert(!buffers.empty());

    alBufferData(buffers.back(), sound->format, from, num, sound->samples_per_second);
    alSourceQueueBuffers(sound->handle, 1, &(buffers.back()));

    buffers.pop_back();
}

bool Player::supportsFloat32(){
    return alIsExtensionPresent("AL_EXT_FLOAT32");
}

}
