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
        JS_FN("reset", RewindSound, 0, 0),
        JS_FN("rewind", RewindSound, 0, 0),
        JS_FN("setRepeat", SoundRepeatSetter, 0, 0),
        JS_FN("getRepeat", SoundRepeatGetter, 0, 0),
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
        //sf_command(sound_file, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);
        
        if(!sound_file){
            Turbo::SetError(ctx, std::string(BRACKNAME " SoundCtor Error could not open file ") + file.string);
            return false;
        }
        
        short buffer[0x10000];
        Sound *sound = new Sound(player.load(nullptr, 0, info.channels, info.samplerate, info.frames));
        
        int iters = 0;
        
        while(unsigned long this_read = sf_read_short(sound_file, buffer, 0x10000)){
            player.addToSound(sound, buffer, SamplesToBytes(this_read));
            iters ++;
        }
        
        printf(BRACKNAME " SoundCtor Info loaded file %s in %i iterations\n", file.string, iters);
        
        sf_close(sound_file);
        
        args.rval().set(OBJECT_TO_JSVAL(sound_proto.wrap(ctx, sound)));
        
        return true;
    }

    bool SoundRepeatGetter(JSContext *ctx, unsigned argc, JS::Value *vp){
        
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        Sound *sound = sound_proto.getSelf(ctx, vp, &args);
    
        args.rval().set(BOOLEAN_TO_JSVAL(sound->getLooping()));
        return true;
    }
    
    bool SoundRepeatSetter(JSContext *ctx, unsigned argc, JS::Value *vp){
        
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        Sound *sound = sound_proto.getSelf(ctx, vp, &args);
        
        if(!Turbo::CheckForSingleArg(ctx, args, Turbo::Bool, __func__))
            return false;
            
        sound->setLooping(args[0].toBoolean());
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
        
        sound->play();
        return true;    
    }
    
    bool PauseSound(JSContext *ctx, unsigned argc, JS::Value *vp){
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        Sound *sound = sound_proto.getSelf(ctx, vp, &args);
        
        sound->pause();
        return true;
    }
    bool StopSound(JSContext *ctx, unsigned argc, JS::Value *vp){
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        Sound *sound = sound_proto.getSelf(ctx, vp, &args);
        
        sound->stop();
        return true;
    }
    bool RewindSound(JSContext *ctx, unsigned argc, JS::Value *vp){
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        Sound *sound = sound_proto.getSelf(ctx, vp, &args);
        
        sound->rewind();
        return true;
    }

}
