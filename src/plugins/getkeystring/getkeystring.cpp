#include <string>
#include <cassert>
#include <jsapi.h>
#include "getkeystring.hpp"

const char * Init(JSContext *ctx, unsigned ID){
    assert(ID>0);
    assert(ctx);
    return "GetKeyString";
}
    
void Close(JSContext *ctx){    
    assert(ctx);
}
    
int NumFunctions(JSContext *ctx){
    assert(ctx);
    return 1;
}
JSNative GetFunction(JSContext *ctx, int n){
    assert(n==0);
    assert(ctx);
    
    return GetKeyString::GetKeyString;
}

const char * GetFunctionName(JSContext *ctx, int n){
    assert(n==0);
    assert(ctx);
    
    return "GetKeyString";
    
}
    
int NumVariables(JSContext *ctx){
    assert(ctx);
    return 0;
}

JS::Heap<JS::Value> *GetVariable(JSContext *ctx, int n){
    assert(ctx);
    assert(false);
    return nullptr;
}

const char * GetVariableName(JSContext *ctx, int n){
    assert(ctx);
    assert(false);
    return nullptr;
}
namespace GetKeyString{
bool GetKeyString(JSContext *ctx, unsigned argc, JS::Value *vp){
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(args.length()==0){
        const char error_str[] = "[GetKeyString] GetKeyString Error requires 1 argument, called with 0";
        JS::RootedValue error(ctx, STRING_TO_JSVAL(JS_NewStringCopyN(ctx, error_str, sizeof(error_str)-1)));
        JS_SetPendingException(ctx, error);
        return false;
    }
    
    if(!args[0].isNumber()){
        const char error_str[] = "[GetKeyString] GetKeyString Error argument 0 is not a string";
        JS::RootedValue error(ctx, STRING_TO_JSVAL(JS_NewStringCopyN(ctx, error_str, sizeof(error_str)-1)));
        JS_SetPendingException(ctx, error);
        return false;
    }
    
    bool shift = false;
    if((args.length()>1) && (!args[1].isBoolean())){
        const char error_str[] = "[GetKeyString] GetKeyString Error argument 1 is not a string";
        JS::RootedValue error(ctx, STRING_TO_JSVAL(JS_NewStringCopyN(ctx, error_str, sizeof(error_str)-1)));
        JS_SetPendingException(ctx, error);
        return false;
    }

    unsigned int keynum = (args[0].isInt32())?(args[0].toInt32()):(args[0].toNumber());
    
    if (keynum>255){
        args.rval().set(JS_GetEmptyStringValue(ctx));
        return true;
    }

    if(shift){
        keynum = toupper(keynum);
    }
    
    char key = static_cast<char>(keynum);
    
    args.rval().set(STRING_TO_JSVAL(JS_NewStringCopyN(ctx, &key, 1)));
    return true;
}
}
