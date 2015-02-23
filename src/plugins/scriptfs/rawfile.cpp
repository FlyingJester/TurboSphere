#include "scriptfs.hpp"
#include "rawfile.hpp"
#include <memory>

namespace scriptfs{

void RawfileFinalizer(JSFreeOp *fop, JSObject *obj){
    delete raw_file_proto.unsafeUnwrap(obj);
}
    
Turbo::JSPrototype<RawFileHolder> raw_file_proto("RawFile", OpenRawFile, 1, RawfileFinalizer);

JSFunctionSpec rawfile_methods[] = {
    JS_FN("read", RawFileRead, 1, 0),
    JS_FN("write", RawFileWrite, 2, 0),
    JS_FN("getSize", RawFileGetSize, 0, 0),
    JS_FN("getPosition", RawFileGetPosition, 0, 0),
    JS_FN("setPosition", RawFileSetPosition, 1, 0),
    JS_FS_END
};

JSPropertySpec rawfile_properties[] = {
    JS_PSG("size", RawFileGetSize, 0),
    JS_PSGS("position", RawFileGetPosition, RawFileSetPosition, 0),
    JS_PS_END
};

}


RawFileHolder::RawFileHolder(a _a){
    A = _a;
}

RawFileHolder::~RawFileHolder(){
    CloseRawFile(A);
    FreeRawFile(A);
}

void RawFileHolder::forget(){A = nullptr;}

void RawFileHolder::operator = (const a _a){
    forget();
    A = _a;
}

void InitRawFile(JSContext *ctx, int ID){
    scriptfs::raw_file_proto.initForContext(ctx, scriptfs::rawfile_properties, scriptfs::rawfile_methods);
}

void CloseRawFile(JSContext *ctx){
    scriptfs::raw_file_proto.closeForContext(ctx);
}

const char * const ExplainRawFileError(enum RawFileError aError){
    switch(aError){
        case RF_NoError:
            return "No error.";
        case RF_NotPath:
            return "Bad path.";
        case RF_NotOpen:
            return "File not open.";
        case RF_NotWriteable:
            return "Attempted to write to a read-only file.";
        case RF_EOF:
            return "End of file.";
        case RF_Unkown:
            return "Unkown error.";
        default:
            return "Invalid error.";
    }
    return "The impossible has happened.";
}

