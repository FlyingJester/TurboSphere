#include "scriptfs.hpp"
#include "rawfile.hpp"
#include <memory>

namespace scriptfs{

Turbo::JSPrototype<RawFileHolder> RawFileProto("RawFile",  scriptfs::OpenRawFile);

}

RawFileHolder::RawFileHolder(a _a){
    A = _a;
}

RawFileHolder::~RawFileHolder(){
    FreeRawFile(A);
    CloseRawFile(A);
}

void RawFileHolder::forget(){A = nullptr;}

void RawFileHolder::operator = (const a _a){
    forget();
    A = _a;
}

void InitRawFile(JSContext *ctx, int ID){
    scriptfs::RawFileProto.initForContext(ctx);
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

