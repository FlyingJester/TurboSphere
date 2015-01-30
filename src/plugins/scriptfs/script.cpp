#include <string>
#include <vector>
#include <jsfriendapi.h>
#include <t5.h>
#include <opengame.h>
#include "script.hpp"

namespace scriptfs {
    
    
    template<bool(*func)(const char *)>
    bool t5Call(JSContext *ctx, unsigned argc, JS::Value *vp){
        
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        const Turbo::JSType signature[] = {Turbo::Number};
        
        if(!Turbo::CheckSignature<1>(ctx, args, signature, __func__))
            return false;
        
        char *file = JS_EncodeString(ctx, args[0].toString());
        if(!file){
            Turbo::SetError(ctx, "could not encode string for argument 0");
            return false;
        }
        
        std::string full_path = std::string(TS_GetContextEnvironment(ctx)->directories->root)+file;
        
        JS_free(ctx, file);
        
        args.rval().set(BOOLEAN_TO_JSVAL(t5::IsFile(full_path)));
        return true;
    }

    bool IsFile(JSContext *ctx, unsigned argc, JS::Value *vp){
        return t5Call<t5::IsFile>(ctx, argc, vp);
    }
    bool IsDir(JSContext *ctx, unsigned argc, JS::Value *vp){
        return t5Call<t5::IsFile>(ctx, argc, vp);
    }
    
    bool RawFileGetSize(JSContext *ctx, unsigned argc, JS::Value *vp){
        
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        const RawFileHolder *that = RawFileProto.getSelf(ctx, vp, &args);
        if(!that)
            return false;
        
        unsigned long s = 0;

        rawfile_getSize(that->A, &s);

        args.rval().set(JS_NumberValue(s));
        return true;
    }

    bool RawFileGetPosition(JSContext *ctx, unsigned argc, JS::Value *vp){

        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        const RawFileHolder *that = RawFileProto.getSelf(ctx, vp, &args);
        
        if(!that)
            return false;
            
        unsigned long s = 0;

        rawfile_getPosition(that->A, &s);

        args.rval().set(JS_NumberValue(s));
        return true;
    }

    bool RawFileSetPosition(JSContext *ctx, unsigned argc, JS::Value *vp){
     
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        const RawFileHolder *that = RawFileProto.getSelf(ctx, vp, &args);
        if(!that)
            return false;
        
        const Turbo::JSType signature[] = {Turbo::Number};
        
        if(!Turbo::CheckSignature<1>(ctx, args, signature, __func__))
            return false;
        
        unsigned long s = args[0].toNumber();

        rawfile_setPosition(that->A,  s);
        rawfile_getPosition(that->A, &s);

        args.rval().set(JS_NumberValue(s));
        return true;
    }


    bool RawFileRead(JSContext *ctx, unsigned argc, JS::Value *vp){

        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        const RawFileHolder *that = RawFileProto.getSelf(ctx, vp, &args);
        if(!that)
            return false;
            
        static const Turbo::JSType signature[] = {Turbo::Number};
        
        if(!Turbo::CheckSignature<1>(ctx, args, signature, __func__))
            return false;

        unsigned long at, total, left, len = args[0].toNumber();

        enum RawFileError err = rawfile_getPosition(that->A, &at);
        
        if(err!=RF_NoError){
            Turbo::SetError(ctx, ExplainRawFileError(err));
            return false;
        }
        
        err = rawfile_getSize(that->A, &total);

        if(err!=RF_NoError){
            Turbo::SetError(ctx, ExplainRawFileError(err));
            return false;
        }

        left = total-at;
        
        if(len>left){
            Turbo::SetError(ctx, "tried to read past end of RawFile");
            return false;
        }

        std::unique_ptr<void, void(*)(void *)> raw_buffer(malloc(len), free);
        
        err = rawfile_read(that->A, raw_buffer.get(), len);
        
        if(err!=RF_NoError){
            Turbo::SetError(ctx, ExplainRawFileError(err));
            return false;
        }

        JSObject *buffer = JS_NewArrayBufferWithContents(ctx, len, raw_buffer.release());
        
        args.rval().set(OBJECT_TO_JSVAL(buffer));
        
        assert(raw_buffer.get()==nullptr);
        
        return true;
    }

    bool RawFileWrite(JSContext *ctx, unsigned argc, JS::Value *vp){
          
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        
        const RawFileHolder *that = RawFileProto.getSelf(ctx, vp, &args);
        if(!that)
            return false;
            
        const Turbo::JSType signature[] = {Turbo::ArrayBuffer, Turbo::Number};
        
        if(!Turbo::CheckSignature<2>(ctx, args, signature, __func__))
            return false;
        
        JS::RootedObject buffer_root(ctx, &(args[0].toObject())); 
        
        unsigned char *data;
        uint32_t len;
        
        js::GetArrayBufferLengthAndData(buffer_root, &len, &data);
        
        enum RawFileError err = rawfile_write(that->A, data, len);
        
        if(err!=RF_NoError){
            Turbo::SetError(ctx, ExplainRawFileError(err));
            return false;
        }
        
        args.rval().set(JS::UndefinedHandleValue);
        return true;
    }

}

inline bool IsPathSeperator(char c){
    return (c=='/')
#ifdef _WIN32
    || (c=='\\')
#endif
    ;
}

inline bool ReplacePathStringIfFound(std::string &in, const char *replace, const char *with){
    size_t rep_len = strlen(replace);
    size_t c = in.rfind(replace);
    if(c!=std::string::npos){
        in = std::string(with) + in.substr(c+rep_len+1);
        return true;
    }
    return false;
}

bool scriptfs::OpenRawFile(JSContext *ctx, unsigned argc, JS::Value *vp){

    JS::CallArgs args = CallArgsFromVp(argc, vp);
        
    static const Turbo::JSType signature[] = {Turbo::String, Turbo::Number};
    
    if(!Turbo::CheckSignature<1>(ctx, args, signature, __func__))
        return false;
    
    enum RawFileError err;
    bool writeable = false;
    
    char *file = JS_EncodeString(ctx, args[0].toString());
    if(!file){ // How did this happen?
        Turbo::SetError(ctx, "could not encode string for argument 0");
        return false;
    }
    
    std::string full_path = std::string(TS_GetContextEnvironment(ctx)->directories->other)+file;
    
    JS_free(ctx, file);
    
    if(args.length()>1){
        if(!args[1].isBoolean()){
            Turbo::SetError(ctx, "argument 1 is not a Boolean");
            return false;
        }
        writeable = args[1].toBoolean();
    }

    ReplacePathStringIfFound(full_path, "#~", TS_GetContextEnvironment(ctx)->system->system);
    ReplacePathStringIfFound(full_path, "~", TS_GetContextEnvironment(ctx)->directories->root);

    // If we can't write to it and it doesn't exist, this isn't going to work.
    if((!writeable) && (!t5::IsFile(full_path))){
        Turbo::SetError(ctx, "not opening for writing and file does not exist");
        return false;
    }

    struct RawFile *rFile = AllocRawFile();
    assert(rFile);

    err = ::OpenRawFile(rFile, full_path.c_str(), writeable);
    std::unique_ptr<RawFileHolder> holder(new RawFileHolder(rFile));

    if(err!=RF_NoError){
        Turbo::SetError(ctx, ExplainRawFileError(err));
        return false;
    }
    
    args.rval().set(OBJECT_TO_JSVAL(RawFileProto.wrap(ctx, holder.release())));
    return true;
    
}
