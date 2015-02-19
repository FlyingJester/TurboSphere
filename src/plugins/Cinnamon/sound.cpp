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
#include <cmath>
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
      , handle(s.handle) 
      , length(s.length) 
      , num_channels(s.num_channels) 
      , samples_per_second(s.samples_per_second) 
      , format(s.format){
        s.handle = 0u;
        setVolume(s.getVolume());
        setLooping(s.getVolume());
    }

    Sound::Sound(const Sound &s)
      : player(s.player) 
      , handle(s.handle)
      , length(s.length) 
      , num_channels(s.num_channels) 
      , samples_per_second(s.samples_per_second) 
      , format(s.format){
        setVolume(s.getVolume());
        setLooping(s.getVolume());
    }

    void Sound::setLooping(bool loop){
        alSourcei(handle, AL_LOOPING, loop?AL_TRUE:AL_FALSE);
        looping = loop;
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
    
    void Sound::play() const{
        alSourcePlay(handle);
    }
    void Sound::pause() const{
        alSourcePause(handle);
    }
    void Sound::stop() const{
        alSourceStop(handle);
    }
    void Sound::rewind() const{
        alSourceRewind(handle);
    }

    void Sound::setVolume(float to){
        gain = fmin(1.0f, fmax(0.0f, to));
        gain*=gain;
        alSourcef(handle, AL_GAIN, gain);
    }
    
    float Sound::getVolume() const {
        return sqrtf(gain);
    }

}

