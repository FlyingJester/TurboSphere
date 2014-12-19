#include "openal.hpp"

#ifdef USE_OPENAL

#include <cassert>

#ifdef _MSC_VER
#include <malloc.h>
#endif

namespace Cinnamon{

#define ERROR_PRINT \
do{\
    ALenum err = alGetError();\
    if(err!=AL_NO_ERROR){\
        printf(OpenALPlayer::ExplainError_a(err));\
        printf(__FILE__);\
        printf(" line %i\n", __LINE__);\
    }\
}while(0)\

ALCdevice *OpenALPlayer::dev = nullptr;
ALCcontext *OpenALPlayer::context = nullptr;

bool OpenALPlayer::SupportsFloat32(){
    return alIsExtensionPresent("AL_EXT_FLOAT32");
}

void OpenALPlayer::Init(){
    static bool first = true;
    if(first){
        first = false;

        dev = alcOpenDevice(nullptr);

        assert(dev);
        alGetError();

        context = alcCreateContext(dev, nullptr);
        assert(context);

        alcMakeContextCurrent(context);

        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        alListener3f(AL_ORIENTATION, 0.0f, 0.0f, 0.0f);

    }
}

OpenALPlayer::OpenALPlayer(){

    Init();

    alGenSources(1, &(sound.source));

    alSourcef(sound.source, AL_PITCH, 1.0f);
    alSourcef(sound.source, AL_GAIN, 1.0f);
    alSource3f(sound.source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(sound.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(sound.source, AL_LOOPING, AL_FALSE);

}

error_t OpenALPlayer::Load(const uint8_t *from, size_t num, int samples_per_second){

    alcMakeContextCurrent(context);

    if(sound.buffers.empty()){
        ALint num_buffers;
        alGetSourcei(sound.source, AL_BUFFERS_PROCESSED, &num_buffers);

        if(num_buffers==0){
            ALuint new_buffer;
            alGenBuffers(1, &new_buffer);
            sound.buffers.push_back(new_buffer);

        }
        else{
            sound.buffers.resize(sound.buffers.size()+num_buffers);
            alSourceUnqueueBuffers(sound.source, num_buffers, sound.buffers.data()-num_buffers);

        }
    }


    assert(!sound.buffers.empty());

    alBufferData(sound.buffers.back(), al_format, from, num, samples_per_second);
    alSourceQueueBuffers(sound.source, 1, &(sound.buffers.back()));

    sound.buffers.pop_back();

    return eSuccess;

}

}

#endif // USE_OPENAL
