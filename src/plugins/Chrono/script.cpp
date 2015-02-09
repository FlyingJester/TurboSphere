#include "script.hpp"
#include "chrono.hpp"
#include <array>

#define NUM_FUNCS 3

std::array<const char * const, NUM_FUNCS> function_name_list = {{
    "Delay",
    "GetTime",
    "GetSeconds"
}};
std::array<JSNative, NUM_FUNCS> function_list = {{
    Chrono::Delay,
    Chrono::GetTime,
    Chrono::GetSeconds
}};

const char * Init(JSContext *ctx, unsigned ID){
    assert(ctx);
    return PLUGINNAME;
}

void Close(JSContext *ctx){}
    
int NumFunctions(JSContext *ctx){
    assert(ctx);
    assert(function_list.size()==function_name_list.size());
    return function_list.size();
}

JSNative GetFunction(JSContext *ctx, int n){
    assert(ctx);
    assert(n<NUM_FUNCS);
    return function_list[n];
}
const char * GetFunctionName(JSContext *ctx, int n){
    assert(ctx);
    assert(n<NUM_FUNCS);
    return function_name_list[n];
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
const char *GetVariableName(JSContext *ctx, int n){
    assert(ctx);
    assert(false);
    return nullptr;
}
