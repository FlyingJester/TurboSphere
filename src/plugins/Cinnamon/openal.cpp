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

Sound Player::load(const uint8_t *from, size_t num, int samples_per_second){

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

    alBufferData(buffers.back(), format, from, num, samples_per_second);
    alSourceQueueBuffers(sound.handle, 1, &(buffers.back()));

    buffers.pop_back();

    return sound;

}


bool Player::supportsFloat32(){
    return alIsExtensionPresent("AL_EXT_FLOAT32");
}

}
