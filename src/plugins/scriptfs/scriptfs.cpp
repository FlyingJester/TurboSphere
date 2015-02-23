#include <array>
#include <cassert>
#include <t5.h>
#include <openscript.h>
#include <opengame.h>
#include "scriptfs.hpp"
#include "script.hpp"
#include "rawfile.hpp"

namespace scriptfs {

    std::array<JSNative, 2> Functions = {{
       // scriptfs::OpenRawFile,
        scriptfs::IsFile,
        scriptfs::IsDir
    }};

    std::array<const char *, 2> FunctionNames = {{
      // "RawFile",
        "IsFile",
        "IsDirectory",
    }};

}

const char *  Init(JSContext *ctx, unsigned ID){
    assert(ctx);
    assert(ID>0);
    InitRawFile(ctx, ID);
    return "ScriptFS";
    
}

void  Close(JSContext *ctx){
    CloseRawFile(ctx);
    assert(ctx);
}

int  NumFunctions(JSContext *ctx){
    assert(ctx);
    return scriptfs::Functions.size();
}

JSNative GetFunction(JSContext *ctx, int n){
    assert(ctx);
    assert(n<scriptfs::Functions.size());
    
    return scriptfs::Functions[n];
}

const char *  GetFunctionName(JSContext *ctx, int n){
    assert(ctx);
    assert(n<scriptfs::Functions.size());
    
    return scriptfs::FunctionNames[n];
}

int  NumVariables(JSContext *ctx){
    assert(ctx);
    return 0;
}
JS::Heap<JS::Value> * GetVariable(JSContext *ctx, int n){
    assert(ctx);
    assert(false);
    
    return nullptr;
}
const char *  GetVariableName(JSContext *ctx, int n){
    assert(ctx);
    assert(false);
    
    return nullptr;
}
