#include "sound.hpp"
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

namespace Cinnamon {

    Sound::Sound(Player &p)
      : player(p) {
          
        player.makeCurrent();
          
        alGenSources(1, &handle);
        alSourcef(handle, AL_PITCH, 1.0f);
        alSourcef(handle, AL_GAIN, 1.0f);
        alSource3f(handle, AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(handle, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        
        setLooping(false);

    }

    Sound::Sound(Sound &&s)
      : player(s.player) 
      , handle(s.handle){
        s.handle = 0u;
    }

    Sound::Sound(const Sound &s)
      : player(s.player) 
      , handle(s.handle){
    }

    void Sound::setLooping(bool loop){
        alSourcei(handle, AL_LOOPING, loop?AL_TRUE:AL_FALSE);
    }
        
        
    void Sound::returnBuffers() const{
        ALint num_buffers;
        player.makeCurrent();
        
        alGetSourcei(handle, AL_BUFFERS_PROCESSED, &num_buffers);
        if(num_buffers!=0){
            player.buffers.resize(player.buffers.size()+num_buffers);
            alSourceUnqueueBuffers(handle, num_buffers, player.buffers.data()-num_buffers);
        }
        
    }


    Sound::~Sound(){
        returnBuffers();
        
        player.makeCurrent();
        alDeleteSources(1, &handle);
    }

}

