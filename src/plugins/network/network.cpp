#include "network.hpp"
#include "script.hpp"
#include <array>
#include <cassert>

static std::array<JSNative, 4> function_list = {{
    NetPlug::GetLocalName, 
    NetPlug::GetLocalAddress, 
    NetPlug::OpenAddress,
    NetPlug::ListenOnPort
}};

static std::array<const char * const, 4> function_name_list = {{
    "GetLocalName",
    "GetLocalAddress",
    "OpenAddress",
    "ListenOnPort"
}};

static JSFunctionSpec socket_methods[] = {
    JS_FN("isConnected", NetPlug::SocketIsConnected, 0, 0),
    JS_FN("getPendingReadSize", NetPlug::SocketGetPendingReadSize, 0, 0),
    JS_FN("write", NetPlug::SocketWrite, 1, 0),
    JS_FN("read", NetPlug::SocketRead, 1, 0),
    JS_FN("close", NetPlug::SocketClose, 0, 0),
    JS_FS_END
};

const char * Init(JSContext *ctx, unsigned ID){
    assert(ctx);
    assert(function_name_list.size() == function_list.size());
    
    NetPlug::socket_proto.initForContext(ctx, nullptr, socket_methods);
    
    return PLUGINNAME;
}

void Close(JSContext *ctx){
    assert(ctx);
    NetPlug::socket_proto.closeForContext(ctx);
}
    
int NumFunctions(JSContext *ctx){
    assert(ctx);
    return function_name_list.size();
}

JSNative GetFunction(JSContext *ctx, int n){
    assert(ctx);
    assert(n>=0);
    assert(n<function_list.size());
    
    return function_list[n];
}

const char * GetFunctionName(JSContext *ctx, int n){
    assert(ctx);
    assert(n>=0);
    assert(n<function_name_list.size());
    
    return function_name_list[n];
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

const char * GetVariableName(JSContext *ctx, int n){
    assert(ctx);
    assert(n>=0);
    assert(false);
    return nullptr;
}
