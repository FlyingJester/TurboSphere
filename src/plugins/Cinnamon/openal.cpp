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

Sound Player::load(const int16_t *from, int num_channels, size_t num, int samples_per_second){

    makeCurrent();

    Sound sound(*this);

    if(buffers.empty()){
        const unsigned num_new_buffers = 8;
        
        ALuint new_buffers[num_new_buffers];
        alGenBuffers(num_new_buffers, new_buffers);
        
        for(unsigned i = 0; i<num_new_buffers; i++)
            buffers.push_back(new_buffers[i]);
    }


    assert(!buffers.empty());

    unsigned channel_format = format;
    switch(num_channels){
        case 1:
            channel_format = AL_FORMAT_MONO16;
            break;
        case 2:
            channel_format = AL_FORMAT_STEREO16;
            break;
    }

    alBufferData(buffers.back(), channel_format, from, num, samples_per_second);
    alSourceQueueBuffers(sound.handle, 1, &(buffers.back()));

    buffers.pop_back();

    return sound;

}

void Player::play(Sound *sound){
    makeCurrent();
    alSourcePlay(sound->handle);
}

bool Player::supportsFloat32(){
    return alIsExtensionPresent("AL_EXT_FLOAT32");
}

}
