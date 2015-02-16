#include "script.hpp"
#include <sndfile.h>
#include <memory>
#include <t5.h>
#include <opengame.h>

namespace Cinnamon {

    template<typename T>
    inline T BytesToSamples(T in){return in/static_cast<T>(2);}
    template<typename T>
    inline T SamplesToBytes(T in){return in*static_cast<T>(2);}
    
static Player player;

    Turbo::JSPrototype<Sound> sound_proto("Sound", SoundCtor, 3, SoundFinalizer);


    static JSFunctionSpec sound_methods[] = {
        JS_FN("play", PlaySound, 0, 0),
        JS_FN("pause", PauseSound, 0, 0),
        JS_FN("stop", StopSound, 0, 0),
        JS_FS_END
    };

    void InitScript(JSContext *ctx){
        player.init();
        sound_proto.initForContext(ctx, nullptr, sound_methods);
    }

    
    void SoundFinalizer(JSFreeOp *fop, JSObject *obj){
        delete sound_proto.unsafeUnwrap(obj);
    }
    
    
    // TODO: Add streaming support.
    bool SoundCtor(JSContext *ctx, unsigned argc, JS::Value *vp){
        
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        if(!Turbo::CheckForSingleArg(ctx, args, Turbo::String, __func__))
            return false;
                
        struct Turbo::JSStringHolder<> file(ctx, JS_EncodeString(ctx, args[0].toString()));
        
        const std::string full_path = std::string(TS_GetContextEnvironment(ctx)->directories->sound) + file.string;

        if(!t5::IsFile(full_path)){
            Turbo::SetError(ctx, std::string(BRACKNAME " SoundCtor Error no such file ") + file.string);
            return false;
        }
        
        SF_INFO info;
        SNDFILE *sound_file = sf_open(full_path.c_str(), SFM_READ, &info);
        
        if(!sound_file){
            Turbo::SetError(ctx, std::string(BRACKNAME " SoundCtor Error could not open file ") + file.string);
            return false;
        }
        
        std::vector<short> buffer;
        buffer.resize(8192*info.channels);
        unsigned long read_in = 0; // In samples
        
        printf(BRACKNAME " SoundCtor Info %i channels\n", info.channels);
        
        while(unsigned long this_read = sf_readf_short(sound_file, buffer.data()+SamplesToBytes(read_in), BytesToSamples(buffer.size()-SamplesToBytes(read_in))/info.channels)){
            read_in+=this_read;
            buffer.resize(buffer.size()<<1);
        }
        
        sf_close(sound_file);
        
        Sound *sound = new Sound(player.load(buffer.data(), info.channels, SamplesToBytes(read_in), info.samplerate));
        
        args.rval().set(OBJECT_TO_JSVAL(sound_proto.wrap(ctx, sound)));
        
        return true;
    }

    bool PlaySound(JSContext *ctx, unsigned argc, JS::Value *vp){
        
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        Sound *sound = sound_proto.getSelf(ctx, vp, &args);
        
        if(Turbo::CheckForSingleArg(ctx, args, Turbo::Bool, __func__, false)){
            sound->setLooping(args[0].toBoolean());
        }
        else{
            sound->setLooping(false);
        }
        
        player.play(sound);
        return true;    
    }
    
    bool PauseSound(JSContext *ctx, unsigned argc, JS::Value *vp){
        return true;    
    }
    bool StopSound(JSContext *ctx, unsigned argc, JS::Value *vp){
        return true;
    }

}
