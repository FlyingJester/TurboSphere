#pragma once
#ifdef USE_OPENAL
#include <vector>
#include <list>
#include "../libapi.hpp"

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alext.h>
#include <AL/alc.h>
#endif
#include <cassert>

#ifndef AL_FORMAT_MONO_FLOAT32
#define AL_FORMAT_MONO_FLOAT32                   0x10010
#endif
#ifndef AL_FORMAT_STEREO_FLOAT32
#define AL_FORMAT_STEREO_FLOAT32                 0x10011
#endif

namespace Cinnamon {

class OpenALPlayer : public Player {

protected:

    mutable ALenum error;
    inline ALenum GatherError() const {if(error==AL_NO_ERROR) error = alGetError(); return error;}
    inline void ResetError(){error = AL_NO_ERROR;}

    inline error_t CheckError() const {return (GatherError()==AL_NO_ERROR)?eSuccess:eFailure;}
    static bool SupportsFloat32();

    static void Init();

    static ALCdevice *dev;
    static ALCcontext *context;

    struct sound_t{
        ALuint source;
// Buffers that have been allocated for this sound, but are not in use or queued at the moment.
        std::vector<ALuint> buffers;
    } sound;

    std::vector<uint8_t> buffer;

    uint8_t byte_width;
    ALenum al_format;
    uint32_t rate;

public:

    OpenALPlayer();
    OpenALPlayer(style type, int byte_width_, int sample_per_second)
      : OpenALPlayer() {
        if((type==sFloating) && SupportsFloat32()){
            byte_width = 4;
            al_format = AL_FORMAT_MONO_FLOAT32;
        }
        else if(byte_width_>=2){
            byte_width = 2;
            al_format = AL_FORMAT_MONO16;
        }
        else{
            byte_width = 1;
            al_format = AL_FORMAT_MONO16;
        }
        rate = sample_per_second;
    }

    style Style()     const override {return (byte_width==1)?sUnsigned:sSigned;}
    int   ByteWidth() const override {return byte_width;}

    error_t Load(const uint8_t *from, size_t num, int samples_per_second) override;

    error_t Play() override{
        alcMakeContextCurrent(context);
        alSourcePlay(sound.source);
        return CheckError();
    }

    error_t Pause() override{
        alcMakeContextCurrent(context);
        alSourcePause(sound.source);
        return CheckError();
    }

    error_t Stop() override{
        alcMakeContextCurrent(context);
        alSourceStop(sound.source);
        return CheckError();
    }

    error_t Restart() override{
        alcMakeContextCurrent(context);
        alSourceRewind(sound.source);
        return CheckError();
    }

    static const char * const ExplainError_a(ALenum a){

        switch(a){
        case AL_INVALID_NAME:
            return "Bad OpenAL object handle.";
        case AL_INVALID_ENUM:
            return "Bad enum value.";
        case AL_INVALID_VALUE:
            return "Value out of range.";
        case AL_INVALID_OPERATION:
            return "Illegal operation.";
        case AL_OUT_OF_MEMORY:
            return "Out of memory.";
        }

        return "unknown/none";

    }
    const char * const ExplainError() const override {
        return ExplainError_a(GatherError());
    }



};


}

#endif // USE_OPENAL
