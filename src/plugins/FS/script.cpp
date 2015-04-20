#include "script.hpp"
#include "canonicalize.h"
#include <jsfriendapi.h>
#include <t5.h>
#include <opengame.h>
#include <cassert>

namespace RawFile{


static JSFunctionSpec rawfile_methods[] = {
    JS_FN("read", RawFileRead, 1, 0),
    JS_FN("write", RawFileWrite, 1, 0),
    JS_FN("flush", RawFileFlush, 0, 0),
    JS_FN("close", RawFileClose, 0, 0),
    JS_FN("getPosition", RawFileTell, 0, 0),
    JS_FN("setPosition", RawFileSeek, 1, 0),
    JS_FN("getSize", RawFileSize, 0, 0),
    JS_FN("readString", RawFileString, 1, 0),
    JS_FS_END
};


static JSPropertySpec rawfile_properties[] = {
    JS_PSG("size", RawFileSize, 0),
    JS_PSGS("position", RawFileTell, RawFileSeek, 0),
    JS_PS_END
};

static inline bool ReplacePathStringIfFound(std::string &in, const char *replace, const char *with){
    size_t rep_len = strlen(replace);
    size_t c = in.rfind(replace);
    if(c!=std::string::npos){
        in = std::string(with) + in.substr(c+rep_len+1);
        return true;
    }
    return false;
}

static void RawFileFinalizer(JSFreeOp *fop, JSObject *obj){
    
    t5::DataSource *file = raw_file_proto.unsafeUnwrap(obj);
    
    delete file;    

}

Turbo::JSPrototype<t5::DataSource> raw_file_proto("RawFile", OpenRawFile, 1, RawFileFinalizer);

void initScript(JSContext *ctx){
    raw_file_proto.initForContext(ctx, rawfile_properties, rawfile_methods);
}

void closeScript(JSContext *ctx){
    raw_file_proto.closeForContext(ctx);
}

// This is used by RawFileSeek, RawFileRead, and RawFileString
// Always will return false, or return true AND len >=0
bool RawFileGetReadLength(JSContext *ctx, int64_t &len, JS::CallArgs &args, t5::DataSource * const src){
    if(args.length()==0){
        len = src->Length();
    }
    else if(!CheckForSingleArg(ctx, args, Turbo::Number, __func__)){
        return false;
    }
    else{
        len = args[0].toNumber();
    }

    if(len<0){
        Turbo::SetError(ctx, BRACKNAME " RawFileGetReadLength Error amount to read is negative.");
        return false;
    }
    
    if(src->Length()-src->Tell()<len){
        Turbo::SetError(ctx, BRACKNAME " RawFileGetReadLength Error tried to read past the end of the file.");
        printf("From %i to %i (%f) max %i\n", (int)src->Tell(), (int)len+(int)src->Tell(), args[0].toNumber(), (int)src->Length());
        return false;
    }

    return true;
}

bool OpenRawFile(JSContext *ctx, unsigned argc, JS::Value *vp){
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!CheckForSingleArg(ctx, args, Turbo::String, __func__))
        return false;

    bool writeable = false;
        
    if(CheckForSingleArg(ctx, args, Turbo::Bool, __func__, 1, false))
        writeable = args[0].toBoolean();
    
    const char * file_name = JS_EncodeString(ctx, args[0].toString());
    
    std::string canon_path = std::string(TS_GetContextEnvironment(ctx)->directories->other) + file_name;

    ReplacePathStringIfFound(canon_path, "#~", TS_GetContextEnvironment(ctx)->system->system);
    ReplacePathStringIfFound(canon_path, "~", TS_GetContextEnvironment(ctx)->directories->root);
    
    const char *path = TS_CanonicalizePathName(canon_path.c_str());

    printf("Opening file '%s' as '%s', expanded to '%s'\n", file_name, canon_path.c_str(), path);
    
    canon_path = path;
    
    JS_free(ctx, (void *)file_name);
    free((void *)path);
    
    if(!(writeable || t5::IsFile(canon_path))){
        Turbo::SetError(ctx, (std::string(BRACKNAME " OpenRawFile Error file ") + canon_path + (" does not exist and is not opened for writing.")).c_str());
        return false;
    }
    
    int a = t5::DataSource::eRead|(writeable?t5::DataSource::eWrite:0);
    t5::DataSource *data_source = t5::DataSource::FromPath(a, canon_path.c_str());
    
    if(!data_source){
        Turbo::SetError(ctx, (std::string(BRACKNAME " OpenRawFile Error file ") + canon_path + (" cannot be opened.")).c_str());
        return false;
    }
    
    args.rval().set(OBJECT_TO_JSVAL(raw_file_proto.wrap(ctx, data_source)));
    
    return true;
    
}

bool RawFileSeek(JSContext *ctx, unsigned argc, JS::Value *vp){

    JS::CallArgs args = CallArgsFromVp(argc, vp);
     
    if(!CheckForSingleArg(ctx, args, Turbo::Number, __func__))
        return false;

    t5::DataSource *src = raw_file_proto.getSelf(ctx, vp, &args);    
    int64_t len = args[0].toNumber();

    if(len<0){
        Turbo::SetError(ctx, BRACKNAME " RawFileSeek Error position to seek to is negative.");
        return false;
    }
    
    if(src->Length()<len){
        Turbo::SetError(ctx, BRACKNAME " RawFileSeek Error tried to seek past the end of the file.");
        return false;
    }
    
    src->Seek(len, t5::DataSource::eStart);
    
    return true;
}

bool RawFileTell(JSContext *ctx, unsigned argc, JS::Value *vp){
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    t5::DataSource *source = raw_file_proto.getSelf(ctx, vp, &args);
    
    args.rval().set(JS::NumberValue(source->Tell()));

    return true;
}

bool RawFileSize(JSContext *ctx, unsigned argc, JS::Value *vp){

    JS::CallArgs args = CallArgsFromVp(argc, vp);

    t5::DataSource *source = raw_file_proto.getSelf(ctx, vp, &args);
    
    args.rval().set(JS::NumberValue(source->Length()));
  
    return true;
}

bool RawFileRead(JSContext *ctx, unsigned argc, JS::Value *vp){

    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    t5::DataSource * const src = raw_file_proto.getSelf(ctx, vp, &args);
    int64_t len = 0;
    if(!RawFileGetReadLength(ctx, len, args, src))
        return false;
        
    if(len==0){
        args.rval().set(OBJECT_TO_JSVAL(JS_NewArrayBuffer(ctx, 0)));
        return true;
    }
    /* else */
    {
        void * const buffer = malloc(len);
        src->Read(buffer, len);
        args.rval().set(OBJECT_TO_JSVAL(
            JS::RootedObject(ctx, JS_NewArrayBufferWithContents(ctx, len, buffer))
        ));

        return true;
    }
}

bool RawFileString(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    t5::DataSource * const src = raw_file_proto.getSelf(ctx, vp, &args);
    int64_t len = 0;
    if(!RawFileGetReadLength(ctx, len, args, src))
        return false;

    if(len==0){
        args.rval().set(JS_GetEmptyStringValue(ctx));
        return true;
    }
    /* else */
    {    
        void * const buffer = malloc(len);
        src->Read(buffer, len);
        args.rval().set(STRING_TO_JSVAL(JS_NewStringCopyN(ctx, reinterpret_cast<char *>(buffer), len)));
        free(buffer);
        return true;
    }
}

bool RawFileWrite(JSContext *ctx, unsigned argc, JS::Value *vp){
    unsigned char *data;
    unsigned l;
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);

    if(CheckForSingleArg(ctx, args, Turbo::ArrayBuffer, __func__)){
        JS::RootedObject buffer_root(ctx, args[0].toObjectOrNull());
        js::GetArrayBufferLengthAndData(buffer_root, &l, &data);
    }
    else if(CheckForSingleArg(ctx, args, Turbo::TypedArray, __func__)){
        JS::RootedObject buffer_root(ctx, args[0].toObjectOrNull());
        js::GetArrayBufferViewLengthAndData(buffer_root, &l, &data);
    }
    else{
        Turbo::SetError(ctx, BRACKNAME " RawFileWrite Error argument 0 must be an ArrayBuffer, ByteArray, or a TypedArray.");
        return false;
    }
    
    assert(data);
    
    if(l){
        t5::DataSource *src = raw_file_proto.getSelf(ctx, vp, &args);
        src->Write(data, l);
    }
    return true;
}



bool RawFileFlush(JSContext *ctx, unsigned argc, JS::Value *vp){
    return true;
}

bool RawFileClose(JSContext *ctx, unsigned argc, JS::Value *vp){
    return true;
}

}
