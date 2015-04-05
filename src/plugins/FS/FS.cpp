#include "FS.hpp"
#include "script.hpp"

const char * Init(JSContext *ctx, unsigned ID){
    assert(ctx);
    RawFile::initScript(ctx);
    return PLUGINNAME;
}

void Close(JSContext *ctx){
    assert(ctx);
    RawFile::closeScript(ctx);
}

int NumFunctions(JSContext *ctx){
    assert(ctx);
    return 0;
}

JSNative GetFunction(JSContext *ctx, int n){
    assert(ctx);
    assert(false);
    return nullptr;
}

const char *GetFunctionName(JSContext *ctx, int n){
    assert(ctx);
    assert(n>=0);
    assert(false);
    return nullptr;
}

int NumVariables(JSContext *ctx){
    assert(ctx);
    return 0;
}

void GetVariable(JSContext *ctx, int n, JS::MutableHandleValue val){
    assert(ctx);
    assert(n>=0);
    assert(false);
}
const char *GetVariableName(JSContext *ctx, int n){
    assert(ctx);
    assert(n>=0);
    assert(false);
    return nullptr;
}
